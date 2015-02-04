/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#ifdef COMPILER_MSVC
#define LogPrintDebug OutputDebugStringA
#else
#define LogPrintDebug(x) printf("%s", x)
#endif

#include "API.h"
#include "Timer.h"
#include "StringExt.h"
#include "StackWalker.h"

#include <miniformat/miniformat.h>

#include <functional>
#include <vector>
#include <mutex>

namespace dit {

    //-----------------------------------//

    class Log
    {
    public:
        enum class Level : uint32
        {
            Debug
            , Error
            , Info
            , Memory
        };

        struct Callstack
        {
            virtual ~Callstack() {}
            virtual void print() {}
            virtual void clear() {}

            virtual StackWalker::Symbol GetSymbolFromAddr(CONTEXT* ctx, void* addr)
            {
                return StackWalker::Symbol{};
            }

            virtual const char* data() const  { return nullptr; }
        };

        struct Entry
        {
            double time;
            Level lvl;
            String msg;
            String msgLine;
        };

        typedef std::function<void(Entry*)> Handler;

    public:
        Log();
        ~Log();

        void Write(const char* msg, Level lvl)
        {
            std::unique_lock<std::mutex> lock(mutex);

            entry.lvl = lvl;
            entry.time = timer.GetElapsed();

            mini::format(entry.msg, msg);
            mini::format(entry.msgLine, "%0\n", entry.msg.c_str());

            Dispatch();
        }

        template < typename T >
        void Write(const char* msg, Level lvl, T&& a0)
        {
            std::unique_lock<std::mutex> lock(mutex);

            entry.lvl = lvl;
            entry.time = timer.GetElapsed();

            mini::format(entry.msg, msg, std::forward<T>(a0));
            mini::format(entry.msgLine, "%0\n", entry.msg.c_str());

            Dispatch();
        }

        template < typename T, typename... TT >
        void Write(const char* msg, Level lvl, T&& a0, TT&&... an)
        {
            std::unique_lock<std::mutex> lock(mutex);

            entry.lvl = lvl;
            entry.time = timer.GetElapsed();

            mini::format(entry.msg, msg, std::forward<T>(a0), std::forward<TT>(an)...);
            mini::format(entry.msgLine, "%0\n", entry.msg.c_str());

            Dispatch();
        }

    public:

        std::mutex mutex;

        Timer timer;
        Entry entry;
        Callstack* callstack = nullptr;

        std::vector<Handler, Mallocator<Handler>> errorHandlers;
        std::vector<Handler, Mallocator<Handler>> debugHandlers;
        std::vector<Handler, Mallocator<Handler>> infoHandlers;

        HashMap<uint64, Mallocator> prevErrors;

    public:
        // helper function do not call
        void Dispatch()
        {
            switch (entry.lvl)
            {
            case Level::Debug:
                for (auto& fn : debugHandlers)
                    fn(&entry);
                break;
            case Level::Error:
                {
                    auto errHash = MurmurHash64(entry.msg.c_str(), entry.msg.size(), 0);
                    if (!prevErrors.has(errHash))
                    {
                        prevErrors.set(errHash, errHash);

                        for (auto& fn : errorHandlers)
                            fn(&entry);
                    }
                }
                
                break;
            case Level::Info:
                for (auto& fn : infoHandlers)
                    fn(&entry);
                break;
            };
        }

    public:
        static Log* _DefaultLog;
    };

    //-----------------------------------//
    // error logging overloads
    
    void log(const char* msg);

    template < typename T >
    void log(const char* msg, T&& a0)
    {
        auto log = Log::_DefaultLog;
        if (log == nullptr)
            return;

        log->Write(msg, Log::Level::Info, std::forward<T>(a0));
    }

    template < typename T, typename... TT >
    void log(const char* msg, T&& a0, TT&&... an)
    {
        auto log = Log::_DefaultLog;
        if (log == nullptr)
            return;

        log->Write(msg, Log::Level::Info, std::forward<T>(a0), std::forward<TT>(an)...);
    }

    //-----------------------------------//
    // debug logging overloads

    void log_dbg(const char* msg);

    template < typename T >
    void log_dbg(const char* msg, T&& a0)
    {
        auto log = Log::_DefaultLog;
        if (log == nullptr)
            return;

        log->Write(msg, Log::Level::Debug, a0);
    }

    template < typename T, typename... TT >
    void log_dbg(const char* msg, T&& a0, TT&&... an)
    {
        auto log = Log::_DefaultLog;
        if (log == nullptr)
            return;

        log->Write(msg, Log::Level::Debug, std::forward<T>(a0), std::forward<TT>(an)...);
    }

    //-----------------------------------//

    void log_err(const char* msg);

    template < typename T >
    void log_err(const char* msg, T&& a0)
    {
        auto log = Log::_DefaultLog;
        if (log == nullptr)
            return;

        log->Write(msg, Log::Level::Error, std::forward<T>(a0));
    }

    template < typename T, typename... TT >
    void log_err(const char* msg, T&& a0, TT&&... an)
    {
        auto log = Log::_DefaultLog;
        if (log == nullptr)
            return;

        log->Write(msg, Log::Level::Error, std::forward<T>(a0), std::forward<TT>(an)...);
    }

    //-----------------------------------//

    void log_callstack();
}