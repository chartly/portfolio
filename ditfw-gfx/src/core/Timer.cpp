/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "Timer.h"

#if defined(PLATFORM_MACOSX)
#pragma TODO("OSX: Use Mach timers: http://developer.apple.com/library/mac/#qa/qa2004/qa1398.html")
#elif defined(PLATFORM_NACL)
#include <ppapi/c/pp_time.h>
#elif defined(PLATFORM_LINUX)
#include <sys/time.h>
#define tv_time_ms(t) ((t.tv_sec * 1000000.0) + t.tv_usec)
#pragma TODO("Linux: http://linux.die.net/man/3/clock_gettime")
#elif defined(PLATFORM_WINDOWS)
#include <windows.h>
#endif

namespace dit {

    //-----------------------------------//

#ifdef PLATFORM_WINDOWS

    CPUFreq::CPUFreq()
    {
        Compute();
    }

    void CPUFreq::Start()
    {
        QueryPerformanceFrequency(&qpcFreq);

        startCycles = __rdtsc();
        QueryPerformanceCounter(&startTime);
    }

    void CPUFreq::Stop()
    {
        endCycles = __rdtsc();
        QueryPerformanceCounter(&endTime);

        cpuHz = (endCycles - startCycles) * qpcFreq.QuadPart / (endTime.QuadPart - startTime.QuadPart);
    }

    void CPUFreq::Compute()
    {
        Start();
        Sleep(1);
        Stop();
    }

    static int64 GetTime()
    {
        LARGE_INTEGER time;
        QueryPerformanceCounter(&time);
        return time.QuadPart;
    }

#elif defined(PLATFORM_NACL)

    PP_Time GetTime();

#elif PLATFORM_LINUX

    static timeval GetTime()
    {
        timeval time;
        gettimeofday(&time, nullptr);
        return time;
    }

#endif

    //-----------------------------------//

    CPUFreq Timer::_cpuFreq = CPUFreq{};

    //-----------------------------------//

    Timer::Timer()
    {
        Reset();
    }

    //-----------------------------------//

    double Timer::GetCurrent()
    {
#ifdef PLATFORM_WINDOWS
        int64 time = GetTime();
        return double(time) / double(_cpuFreq.cpuHz);
#elif defined(PLATFORM_NACL)
        PP_Time time = GetTime();
        return time;
#else
        timeval time = GetTime();
        return tv_time_ms(time);
#endif
    }

    //-----------------------------------//

    void Timer::Reset()
    {
        time = GetTime();
    }

    //-----------------------------------//

    double Timer::GetElapsed()
    {
#ifdef PLATFORM_WINDOWS
        int64 diff = GetTime() - time;
        return double(diff) / double(_cpuFreq.cpuHz);
#elif defined(PLATFORM_NACL)
        int64 diff = GetTime() - time;
        return diff;
#else
        return tv_time_ms(currentTime) - tv_time_ms(lastTime.tv_sec);
#endif
    }

    //-----------------------------------//

}