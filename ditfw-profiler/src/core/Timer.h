/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"

#ifdef PLATFORM_WINDOWS
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#endif

namespace dit {

    //-----------------------------------//

    class CPUFreq
    {
    public:
        CPUFreq();

        void Start();
        void Stop();
        void Compute();

    public:
        uint64 cpuHz;
        uint64 startCycles;
        uint64 endCycles;

#ifdef PLATFORM_WINDOWS
        LARGE_INTEGER qpcFreq;
        LARGE_INTEGER startTime;
        LARGE_INTEGER endTime;
#else
#pragma TODO("Add support for platforms other than Windows.")
#endif
    };

    //-----------------------------------//

    class Timer
    {
    public:

        Timer();

        void Reset();

        // get elapsed time since timer creation or last reset.
        double GetElapsed();
        double GetCurrent();

        int64 time; //!< time of timer creation

        static CPUFreq _cpuFreq;
    };

    //-----------------------------------//
}