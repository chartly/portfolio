/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"

namespace dit {

    //-----------------------------------//
    // Forward decls

    class ProfilerInternal;

    //-----------------------------------//
    // Publicly exposed profiler interface

    class Profiler
    {
    public:
        //-----------------------------------//

        struct SampleNode
        {
            // null index value
            static const size_t NullIdx;

            SampleNode() = default;
            SampleNode(uint64 _key, size_t _nextIdx)
                : key(_key)
                , nextIdx(_nextIdx)
            {}

            size_t nextIdx = NullIdx;
            uint64 key = 0;
        };

        //-----------------------------------//

        struct Sample
        {
            Sample() = default;
            Sample(void* framePtr, uint64 invocationCount, double time)
                : addr(framePtr)
                , callCount(invocationCount)
                , totalTime(time)
            {}

            // data / stats
            void* addr = nullptr;
            uint64 callCount = 0;
            uint64 instructionCount = 0;
            double totalTime = 0.f;

            // dependency graph links
            size_t childsIdx = SampleNode::NullIdx;
        };

    public:
        //-----------------------------------//

        Profiler(const char* logFileName = "profile-stats.log");
        ~Profiler();

        void Start();
        void Stop();

        void Enter(int64 rdtsc, void* addr);
        void Exit(int64 rdtsc, void* addr);

    private:
        ProfilerInternal*   _prfl = nullptr;
        bool                _isActive = false;

    private:
        static Profiler* _instance;
        friend Profiler* profiler();
    };

    //-----------------------------------//

    Profiler* profiler();
}