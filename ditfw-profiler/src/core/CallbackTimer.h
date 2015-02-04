/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "Mallocator.h"

#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace dit {

    //-----------------------------------//
    // Threaded callback timer manager.
    //
    // Callbacks are executed in a child thread. Callbacks are
    //  periodically invoked at a specified hz. A mutex reference
    //  is provided on construction, so that the CallbackTimer can
    //  manage a synchronization object that callbacks rely on.

    class CallbackTimer
    {
        //-----------------------------------//
        // Forward decls
    public:
        struct Tick;

    public:
        //-----------------------------------//
        // Internal types

        typedef std::function<void(Tick&)> callable_t;
        typedef std::chrono::high_resolution_clock clock_t;
        typedef std::vector<Tick, Mallocator<Tick>> vector_t;
        typedef std::reference_wrapper<std::mutex> mutex_t;

        //-----------------------------------//
        // Timer info data structure

        struct Tick
        {
            size_t idx = 0;
            float hz = 0;
            clock_t::time_point timePoint;
            callable_t callable;

            template<typename CallableT>
            Tick(size_t idx_, float hz_, clock_t::time_point timePoint_, CallableT callable_)
                : idx(idx_)
                , hz(hz_)
                , timePoint(timePoint_)
                , callable(std::forward<CallableT>(callable_))
            {}

            Tick(Tick && other)
                : idx(other.idx)
                , hz(other.hz)
                , timePoint(other.timePoint)
                , callable(std::move(other.callable))
            {}
        };

        //-----------------------------------//
        
    public:

        CallbackTimer(mutex_t mutex);
        ~CallbackTimer();

        //-----------------------------------//
        // Main thread
        void Start(size_t idx, float hz, callable_t && callable);
        void Start(size_t idx, size_t milliSeconds, callable_t && callable);
        void Stop(size_t idx);

        void Halt();

        //-----------------------------------//
        // Child thread
        void Execute(Tick& timer);
        void AsyncLoop();

    private:
        void Copy(clock_t::time_point& now);

        //-----------------------------------//
    public:

        mutex_t                             mutex;
        std::atomic<bool>                   shouldStop{ false };
        bool                                shouldSort{ false };
        
        std::unique_ptr<std::thread>        thread;
        std::condition_variable             condition;
        vector_t                            rootTimers;
        vector_t                            currTimers;
    };

    //-----------------------------------//

    template<typename T>
    void start_timer(CallbackTimer& timer, size_t idx, float hz, T callable)
    {
        CallbackTimer::callable_t f{ std::forward<T>(callable) };
        timer.Start(idx, hz, std::move(f));
    }

    //-----------------------------------//

    template<typename T>
    void start_timer(CallbackTimer& timer, size_t idx, int32 milliSeconds, T callable)
    {
        CallbackTimer::callable_t f{ std::forward<T>(callable) };
        timer.Start(idx, (size_t) milliSeconds, std::move(f));
    }

    //-----------------------------------//

    void stop_timer(CallbackTimer& timer, size_t idx);
}