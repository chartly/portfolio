/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "Log.h"

#include <array>

#ifdef PLATFORM_WINDOWS
#include "StackWalker.h"
#endif

namespace dit {

    //-----------------------------------//

    Log* Log::_DefaultLog = nullptr;

    //-----------------------------------//

#ifdef PLATFORM_WINDOWS

    class StackFormatter : public StackWalker
    {
    public:
        virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry)
        {
            entryCount += 1;
            if (entryCount < 4)
                return;

            uint32 nxtIdx = (idx + 1) % 2;
            mini::format(msgs[nxtIdx], "%0\n%1 (%2): %3", msgs[idx].c_str(), entry.lineFileName, (uint32) entry.lineNumber, entry.name);
            idx = nxtIdx;
        }

        std::array<String, 2> msgs;
        uint32 idx = 0;
        uint32 entryCount = 0;
    };

    class WindowsCallstack : public Log::Callstack
    {
    public:

        virtual void print()
        {
            stack.ShowCallstack();
        }

        virtual void clear()
        {
            for (auto& str : stack.msgs)
                str.clear();
            stack.entryCount = 0;
        }

        virtual const char* data() const
        {
            return stack.msgs[stack.idx].c_str();
        }

        virtual StackWalker::Symbol GetSymbolFromAddr(CONTEXT* ctx, void* addr)
        {
            return stack.GetSymbolFromAddr(ctx, addr);
        }

        StackFormatter stack;
    };

#endif

    //-----------------------------------//

    Log::Log()
    {
        if (_DefaultLog == nullptr)
            _DefaultLog = this;

#ifdef PLATFORM_WINDOWS
        auto p = std::malloc(sizeof(WindowsCallstack));
        auto cs = new (p) WindowsCallstack{};
        cs->stack.LoadModules();

        callstack = cs;
#endif
    }

    Log::~Log()
    {
        if (_DefaultLog == this)
            _DefaultLog = nullptr;

        // manually invoke the dtor & free
        callstack->~Callstack();
        std::free(callstack);

        callstack = nullptr;
    }

    //-----------------------------------//

    void log(const char* msg)
    {
        auto log = Log::_DefaultLog;
        if (log == nullptr)
            return;

        log->Write(msg, Log::Level::Info);
    }

    //-----------------------------------//

    void log_dbg(const char* msg)
    {
        auto log = Log::_DefaultLog;
        if (log == nullptr)
            return;

        log->Write(msg, Log::Level::Debug);
    }

    //-----------------------------------//

    void log_err(const char* msg)
    {
        auto log = Log::_DefaultLog;
        if (log == nullptr)
            return;

        log->Write(msg, Log::Level::Error);
    }

    //-----------------------------------//

    void log_callstack()
    {
        auto log = Log::_DefaultLog;
        if (log == nullptr || log->callstack == nullptr)
            return;

        log->callstack->print();
        log->Write(log->callstack->data(), Log::Level::Info);
        log->callstack->clear();
    }
}