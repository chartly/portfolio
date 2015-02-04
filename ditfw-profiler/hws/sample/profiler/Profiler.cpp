/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "Profiler.h"
#include "Core.h"
#include "CallbackTimer.h"
#include "readerwriterqueue.h"

#include <array>
#include <memory>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <numeric>
#include <ctime>
#include <sstream>
#include <iomanip> // put_time
#include <fstream>

#pragma region Profiler internals

namespace dit {

    //-----------------------------------//

    __declspec(thread) bool _IsCapturingThread = false;

    //-----------------------------------//

    using namespace moodycamel;

    //-----------------------------------//

    class InstrumentedProfiler
    {
    public:
        typedef Profiler::Sample Sample;
        typedef Profiler::SampleNode SampleNode;

        //-----------------------------------//
        // Base sample data

        struct BaseSample
        {
            BaseSample() = default;
            BaseSample(int32 depthIncrement, void* framePtr, int64 clockCount)
                : depthInc(depthIncrement)
                , addr(framePtr)
                , rdtsc(clockCount)
            {}

            int32 depthInc = 0;
            void* addr = nullptr;
            int64 rdtsc = 0;
        };

        //-----------------------------------//
        // Partial sample data

        struct PartialSample
        {
            PartialSample() = default;
            PartialSample(void* _addr, int64 _rdtsc, uint64 _key, uint64 _recurseCount)
                : addr(_addr)
                , rdtsc(_rdtsc)
                , key(_key)
                , recurseCount(_recurseCount)
            {}

            void* addr = nullptr;
            int64 rdtsc = 0;
            uint64 key = 0;
            uint64 recurseCount = 0;
        };

        //-----------------------------------//
        // Sample hash data

        struct Hash
        {
            Hash() = default;
            Hash(void* framePtr, void* parentPtr, int64 _depth)
                : addr(framePtr)
                , parent(parentPtr)
                , depth(_depth)
            {}

            static uint64 Compute(const Hash& sample)
            {
                auto data = reinterpret_cast<const char*>(&sample);
                return MurmurHash64(data, sizeof(Hash), 0);
            }

            void* addr = nullptr;
            void* parent = nullptr;
            int64 depth = 0;
        };

        //-----------------------------------//

        typedef HashMap<Sample, Mallocator> SampleMap;
        typedef std::vector<Sample, Mallocator<Sample>> SampleBuffer;
        typedef std::vector<SampleNode, Mallocator<SampleNode>> NodeBuffer;
        typedef std::vector<uint64, Mallocator<uint64>> KeyBuffer;
        typedef std::vector<PartialSample, Mallocator<PartialSample>> CallStackBuffer;

        //-----------------------------------//

    public:
        InstrumentedProfiler(size_t stackSize)
            : callbackTimer(mutex)
        {
            callStack.reserve(stackSize);
            isRunning = true;

            start_timer(callbackTimer, 0, 8, std::bind(&InstrumentedProfiler::Process, this));
        }

        ~InstrumentedProfiler()
        {
            if (isRunning)
                Halt();
        }

        //-----------------------------------//
        // Producer-thread

        void Push(BaseSample&& block)
        {
            assert(block.addr != nullptr);
            assert(block.depthInc == -1 || block.depthInc == 1);

            pipe.enqueue(block);
        }

        void Halt()
        {
            if (!isRunning)
                return;

            isRunning = false;
        }

        std::thread Serialize()
        {
            std::unique_lock<std::mutex> lock(mutex);

            SampleMap _samples{ samples };
            NodeBuffer _nodes{ nodes };
            KeyBuffer _roots{ roots };

            lock.unlock();

            auto thrFn = std::bind(&InstrumentedProfiler::SerializeJSON, std::move(_samples), std::move(_nodes), std::move(_roots));
            std::thread thr{ thrFn };

            return thr;
        }

        //-----------------------------------//
        // Consumer-thread

        void Process()
        {
            if (!isRunning && pipe.size_approx() == 0)
                return;

            BaseSample sample;
            while (pipe.try_dequeue(sample))
            {
                assert(sample.depthInc == 1 || sample.depthInc == -1);
                assert(sample.addr != nullptr);

                // pushing a new function call onto the stack
                if (sample.depthInc > 0)
                {
                    // child of main()
                    if (rootSampleKey == 0)
                    {
                        assert(sample.depthInc > 0);

                        rootSampleKey = Hash::Compute({ sample.addr, nullptr, 0 });

                        if (!samples.has(rootSampleKey))
                            roots.push_back(rootSampleKey);

                        auto rootSample = samples.get(rootSampleKey, { sample.addr, 0, 0.0 });

                        rootSample.callCount += 1;
                        samples.set(rootSampleKey, rootSample);

                        callStack.push_back({ sample.addr, sample.rdtsc, rootSampleKey, 0 });
                        depth += 1;
                    }

                    // recursive call
                    else if (sample.addr == callStack[depth - 1].addr)
                    {
                        // we must go deeper...
                        auto childKey = Hash::Compute({ sample.addr, sample.addr, (int64) depth });
                        auto childSample = samples.get(childKey, { sample.addr, 0, 0.0 });

                        childSample.callCount += 1;
                        samples.set(childKey, childSample);

                        // first recursive call
                        if (callStack[depth - 1].recurseCount < 2)
                        {
                            assert(depth > 0 && callStack[depth - 1].key);

                            // push onto the head of current sample's child node list
                            auto currSample = samples.get(callStack[depth - 1].key, { sample.addr, 0, 0.0 });
                            auto childNodeIdx = nodes.size();

                            nodes.push_back({ childKey, currSample.childsIdx });
                            currSample.childsIdx = childNodeIdx;

                            // update parent sample in db
                            samples.set(callStack[depth - 1].key, currSample);

                            // push call stack
                            callStack.push_back({ sample.addr, sample.rdtsc, childKey, callStack[depth - 1].recurseCount + 1 });
                            depth += 1;
                        }
                        else
                        {
                            // increment call count
                            auto currSample = samples.get(callStack[depth - 1].key, { sample.addr, 0, 0.0 });
                            currSample.callCount += 1;
                            samples.set(callStack[depth - 1].key, currSample);


                            // increment recursive depth count
                            callStack[depth - 1].recurseCount += 1;
                        }
                    }

                    // invoking a new child function
                    else
                    {
                        auto childKey = Hash::Compute({ sample.addr, callStack[depth - 1].addr, (int64) depth });

                        if (!samples.has(childKey))
                        {
                            assert(samples.has(callStack[depth - 1].key));

                            // push onto head of the parent's childs linked list
                            auto currSample = samples.get(callStack[depth - 1].key, { sample.addr, 0, 0.0 });
                            auto childNodeIdx = nodes.size();

                            nodes.push_back({ childKey, currSample.childsIdx });
                            currSample.childsIdx = childNodeIdx;

                            // update parent sample in db
                            samples.set(callStack[depth - 1].key, currSample);
                        }

                        // update child sample in db
                        auto childSample = samples.get(childKey, { sample.addr, 0, 0.0 });
                        childSample.callCount += 1;
                        samples.set(childKey, childSample);

                        // push call stack
                        callStack.push_back({ sample.addr, sample.rdtsc, childKey, 0 });
                        depth += 1;
                    }
                }

                // returning from a function call
                else
                {
                    // recursive invocation
                    if (callStack[depth - 1].recurseCount > 1)
                    {
                        // update sample in callstack
                        callStack[depth - 1].recurseCount -= 1;

                        // end of call sequence
                        if (callStack[depth - 1].recurseCount == 1)
                        {
                            auto childKey = Hash::Compute({ callStack[depth - 1].addr, callStack[depth - 2].addr, (int64) (depth - 1) });

                            assert(samples.has(childKey));

                            // compute statistics
                            auto clockCycles = sample.rdtsc - callStack[depth - 1].rdtsc;
                            auto executionTime = (double) clockCycles / (double) cpuFreq.cpuHz;
                            uint64 instrCount = (uint64) sample.addr - (uint64) callStack[depth - 1].addr;

                            // update sample database
                            auto childSample = samples.get(childKey, { sample.addr, 0, 0.0 });
                            childSample.totalTime += executionTime;
                            childSample.instructionCount = instrCount;

                            samples.set(childKey, childSample);

                            // pop call stack
                            callStack.pop_back();
                            depth -= 1;
                        }
                    }

                    // returning from a child call
                    else
                    {
                        assert(depth > 0 && samples.has(callStack[depth - 1].key));

                        // compute statistics
                        auto clockCycles = sample.rdtsc - callStack[depth - 1].rdtsc;
                        auto executionTime = (double) clockCycles / (double) cpuFreq.cpuHz;
                        uint64 instrCount = (uint64) sample.addr - (uint64) callStack[depth - 1].addr;

                        // update curr sample in database
                        auto currSample = samples.get(callStack[depth - 1].key, { sample.addr, 0, 0.0 });
                        currSample.totalTime += executionTime;
                        currSample.instructionCount = instrCount;

                        samples.set(callStack[depth - 1].key, currSample);

                        // pop call stack
                        callStack.pop_back();
                        depth -= 1;

                        // returning from current root sample
                        if (depth < 1)
                        {
                            // next instrumented sample is going to be
                            //  pushing a new child call from main(), so
                            //  set the current root sample key back to 0
                            rootSampleKey = 0;
                        }
                    }
                }
            }
        }

        static void SerializeJSON(SampleMap samples, NodeBuffer nodes, KeyBuffer roots)
        {
            // format an appropriate file name
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);

            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H-%M-%S");

            String fileName;
            mini::format(fileName, "profile_stats-%0.json", ss.str().c_str());

            log("logging profiling data  .... \"%0\"", fileName.c_str());

            CONTEXT ctx;
            // get the current thread context
            // ...

            std::array<String, 2> strs;
            size_t strIdx = 0;

            strs[strIdx] = "{ \"rootSamples\" : [ ";
            strIdx = (strIdx + 1) % 2;

            SampleBuffer rootSamples;
            rootSamples.reserve(roots.size());

            for (auto key : roots)
            {
                auto sample = samples.get(key, {});
                rootSamples.push_back(sample);
            }

            std::sort(std::begin(rootSamples), std::end(rootSamples), [](const Sample& lhs, const Sample& rhs) -> bool {
                return lhs.totalTime > rhs.totalTime;
            });

            size_t count = 0;
            for (auto sample : rootSamples)
            {
                auto objStr = SerializeJSONObject(&ctx, samples, nodes, sample);

                mini::format(strs[strIdx], "%0%1", strs[(strIdx + 1) % 2].c_str(), objStr.c_str());
                strIdx = (strIdx + 1) % 2;

                count += 1;
                if (count < roots.size())
                {
                    mini::format(strs[strIdx], "%0, ", strs[(strIdx + 1) % 2].c_str());
                    strIdx = (strIdx + 1) % 2;
                }
            }

            mini::format(strs[strIdx], "%0 ] }", strs[(strIdx + 1) % 2].c_str());

            // dump json string to file
            std::ofstream logFile{ fileName.c_str() };

            if (!logFile.is_open())
            {
                log(" .... failed.");
                return;
            }

            logFile << strs[strIdx].c_str();
            logFile.close();

            log(" .... done.");
        }

        static String SerializeJSONObject(CONTEXT* ctx, SampleMap& samples, NodeBuffer& nodes, Sample sample)
        {
            // get sample / childs data
            SampleBuffer childs;

            auto childIdx = sample.childsIdx;
            while (childIdx != SampleNode::NullIdx)
            {
                auto node = nodes[childIdx];
                auto data = samples.get(node.key, {});

                childs.push_back(data);

                childIdx = node.nextIdx;
            }

            std::sort(std::begin(childs), std::end(childs), [](const Sample& lhs, const Sample& rhs) -> bool {
                return lhs.totalTime > rhs.totalTime;
            });

            double childsTotalTime = 0;
            for (const auto& child : childs)
                childsTotalTime += child.totalTime;

            // inclusive & exclusive timings
            double inclTime = sample.totalTime / (double) sample.callCount;
            double exclTime = (sample.totalTime - childsTotalTime) / (double) sample.callCount;

            // symbol info
            std::unique_lock<std::mutex> logLock{ core()->logger.mutex };
            auto cs = core()->logger.callstack;
            auto sym = cs->GetSymbolFromAddr(ctx, sample.addr);
            logLock.unlock();

            const char* symName = "null";
            const char* fileName = "null";

            if (sym.sym.Name != nullptr)
                symName = sym.name.c_str();
            if (sym.line.FileName != nullptr)
            {
                String fn{ sym.line.FileName };
                std::replace(std::begin(fn), std::end(fn), '\\', '/');

                fileName = fn.c_str();
            }
                

            // json array of child objects
            String childStr{ "[]" };
            if (childs.size() > 0)
            {
                std::array<String, 2> strs;
                size_t strIdx = 0;

                mini::format(strs[strIdx], "[ ");
                strIdx = (strIdx + 1) % 2;

                size_t count = 0;
                for (auto& child : childs)
                {
                    auto obj = SerializeJSONObject(ctx, samples, nodes, child);

                    mini::format(strs[strIdx], "%0%1", strs[(strIdx + 1) % 2].c_str(), obj.c_str());
                    strIdx = (strIdx + 1) % 2;
                    
                    count += 1;
                    if (count < childs.size())
                    {
                        mini::format(strs[strIdx], "%0, ", strs[(strIdx + 1) % 2].c_str());
                        strIdx = (strIdx + 1) % 2;
                    }
                }

                mini::format(strs[strIdx], "%0 ]", strs[(strIdx + 1) % 2].c_str());
                childStr = std::move(strs[strIdx]);
            }

            String sampleObj;
            mini::format(sampleObj
                , "{ \"symbol\": \"%0\", \"totalTime\" : %8, \"inclusiveTime\": %1, \"exclusiveTime\": %2, \"instructionCount\": %3, \"callCount\": %4, \"fileName\": \"%5\", \"lineNumber\": %6, \"children\": %7 }"
                , symName, inclTime, exclTime, sample.instructionCount, sample.callCount, fileName, (size_t)sym.line.LineNumber, childStr.c_str(), sample.totalTime);

            return sampleObj;
        }

    public:
        //-----------------------------------//
        // Shared data
        std::atomic<bool> isRunning{ false };
        std::mutex mutex;

        ReaderWriterQueue<BaseSample> pipe;
        CallbackTimer callbackTimer;

        CPUFreq cpuFreq;

        //-----------------------------------//
        // Consumer-only data
        size_t depth = 0;
        CallStackBuffer callStack;

        uint64 rootSampleKey = 0;
        SampleMap samples;
        NodeBuffer nodes;
        KeyBuffer roots;
    };

    //-----------------------------------//

    class ProfilerInternal
    {
    public:
        ProfilerInternal()
            : _instr(4096 * 4096)
        {}

    public:
        InstrumentedProfiler _instr;
        std::vector<std::thread> _threads;
    };

    //-----------------------------------//
}

#pragma endregion
#pragma region Profiler implementation

namespace dit {

    //-----------------------------------//

    const size_t Profiler::SampleNode::NullIdx = std::numeric_limits<size_t>::max();

    //-----------------------------------//

    Profiler* profiler()
    {
        return Profiler::_instance;
    }

    //-----------------------------------//

    Profiler* Profiler::_instance = nullptr;

    //-----------------------------------//

    Profiler::Profiler(const char* logFileName)
    {
        _IsCapturingThread = true;

        auto ptr = std::malloc(sizeof(ProfilerInternal));
        _prfl = new (ptr) ProfilerInternal{};

        if (_instance == nullptr)
            _instance = this;
    }

    //-----------------------------------//

    Profiler::~Profiler()
    {
        if (_instance == this)
            _instance = nullptr;

        _prfl->~ProfilerInternal();
        std::free(_prfl);
    }

    //-----------------------------------//

    void Profiler::Start()
    {
        if (!_IsCapturingThread)
            return;

        //_isActive = true;
        //_prfl->_instr.Push({ 1, (void *)_ReturnAddress(), (int64)__rdtsc() });
        //_isActive = false;
    }

    //-----------------------------------//

    void Profiler::Stop()
    {
        if (!_IsCapturingThread)
            return;

        _isActive = true;
        //_prfl->_instr.Push({ -1, (void *) _ReturnAddress(), (int64) __rdtsc() });

        // wait for the pipe to be processed
        _prfl->_instr.Halt();

        // dump the stats to file
        auto thr = _prfl->_instr.Serialize();
        _prfl->_threads.push_back(std::move(thr));

        for (auto& t : _prfl->_threads)
            t.join();
    }

    //-----------------------------------//

    void Profiler::Enter(int64 rdtsc, void* addr)
    {
        if (!_IsCapturingThread || _isActive || !_prfl->_instr.isRunning)
            return;

        _isActive = true;
        _prfl->_instr.Push({ 1, addr, rdtsc });
        _isActive = false;
    }

    //-----------------------------------//

    void Profiler::Exit(int64 rdtsc, void* addr)
    {
        if (!_IsCapturingThread || _isActive || !_prfl->_instr.isRunning)
            return;

        _isActive = true;
        _prfl->_instr.Push({ -1, addr, rdtsc });
        _isActive = false;
    }

    //-----------------------------------//
}

#pragma endregion