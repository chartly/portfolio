/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "CallbackTimer.h"

#include <algorithm>

namespace dit {

    //-----------------------------------//

    void stop_timer(CallbackTimer& timer, size_t idx)
    {
        timer.Stop(idx);
    }

    //-----------------------------------//

    CallbackTimer::CallbackTimer(mutex_t mutex_)
        : mutex(mutex_)
        , shouldStop(false)
        , shouldSort(false)
    {
        thread.reset(new std::thread(std::bind(&CallbackTimer::AsyncLoop, this)));
    }

    //-----------------------------------//

    CallbackTimer::~CallbackTimer()
    {
        Halt();
    }

#pragma region Main thread
    //-----------------------------------//

    void CallbackTimer::Start(size_t idx, float hz, callable_t && callBack)
    {
        float ms = (1.f / ((float) hz)) * 1000.f;

        {
            std::unique_lock<std::mutex> lock(mutex);
            rootTimers.push_back(Tick{ idx, hz, clock_t::now() + std::chrono::milliseconds((uint32) ms), std::forward<callable_t>(callBack) });
            shouldSort = true;
        }

        // wake child thread
        condition.notify_one();
    }

    //-----------------------------------//

    void CallbackTimer::Start(size_t idx, size_t ms, callable_t && callBack)
    {
        float hz = 1.f / (((float) ms) / 1000.f);

        {
            std::unique_lock<std::mutex> lock(mutex);
            rootTimers.push_back(Tick{ idx, hz, clock_t::now() + std::chrono::milliseconds((uint32) ms), std::forward<callable_t>(callBack) });
            shouldSort = true;
        }

        // wake child thread
        condition.notify_one();
    }

    //-----------------------------------//

    void CallbackTimer::Stop(size_t idx)
    {
        {
            std::unique_lock<std::mutex> lock(mutex);

            for (auto it = begin(rootTimers); it != end(rootTimers); ++it)
            {
                if (it->idx == idx)
                {
                    rootTimers.erase(it);
                    break;
                }
            }
        }

        // wake child thread
        condition.notify_one();
    }

    //-----------------------------------//

    void CallbackTimer::Halt()
    {
        shouldStop.store(true);
        condition.notify_all();
        thread->join();
    }

#pragma endregion
#pragma region Child thread

    //-----------------------------------//

    void CallbackTimer::Execute(Tick& timer)
    {
        timer.callable(timer);
    }

    //-----------------------------------//

    void CallbackTimer::AsyncLoop()
    {
        for (;;)
        {
            std::unique_lock<std::mutex>  lock(mutex);

            // sleep thread if no work to do
            auto running = !(shouldStop.load());
            while (running && rootTimers.empty())
                condition.wait(lock);

            // break out on exit
            if (shouldStop)
                return;

            // reorder timers if there's new data
            if (shouldSort)
            {
                // do this on the child thread, so as not to slow down the main thread
                std::sort(begin(rootTimers), end(rootTimers), [](const Tick & ti1, const Tick & ti2) {
                    return ti1.timePoint > ti2.timePoint;
                });

                shouldSort = false;
            }

            auto now = clock_t::now();

            // copy timer list
            Copy(now);

            // calc when next timer should be called
            while (!currTimers.empty())
            {
                auto expire = currTimers.back().timePoint;
                bool callTimer = expire < now;

                // sleep until next timer
                if (!callTimer)
                {
                    auto napTime = expire - now;
                    condition.wait_for(lock, napTime);

                    // double check on wake that there's work
                    if (!rootTimers.empty())
                    {
                        auto now = clock_t::now();
                        Copy(now);

                        auto expire = currTimers.back().timePoint;
                        callTimer = expire < now;
                    }
                }

                // exec the callback
                if (callTimer)
                {
                    Tick timeInfo{ std::move(currTimers.back()) };
                    currTimers.pop_back();

                    Execute(timeInfo);
                }
            }
            
            lock.unlock();
        }
    }

    //-----------------------------------//

    void CallbackTimer::Copy(clock_t::time_point& now)
    {
        currTimers.clear();
        currTimers.reserve(rootTimers.size());

        for (auto& tick : rootTimers)
        {
            // update expiration time
            if (tick.timePoint < now)
            {
                currTimers.push_back(tick);

                float ms = (1.f / tick.hz) * 1000.f;
                tick.timePoint = tick.timePoint + std::chrono::milliseconds((size_t) ms);
            }
        }
    }

    //-----------------------------------//
#pragma endregion

}