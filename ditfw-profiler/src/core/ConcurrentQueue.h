/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"

#include <queue>
#include <mutex>
#include <condition_variable>

namespace dit {

    //-----------------------------------//

    /**
     * Concurrent queue that is safe to use even with multiple producers
     * and consumers. This class is based from code located at website:
     * http://www.justsoftwaresolutions.co.uk/threading/implementing-a-
     * thread-safe-queue-using-condition-variables.html
     */

    template<typename T> class ConcurrentQueue
    {
    public:
        void push(const T& value)
        {
            std::unique_lock<std::mutex> lock{ _mutex };
            
            queue.push(value);

            lock.unlock();

            _cond.notify_one();
        }

        //-----------------------------------//

        bool empty()
        {
            std::unique_lock<std::mutex> lock{ _mutex };

            bool empty = queue.empty();

            return empty;
        }

        //-----------------------------------//

        bool try_pop(T& popped_value)
        {
            std::unique_lock<std::mutex> lock{ _mutex };

            if (queue.empty())
                return false;

            popped_value = queue.front();
            queue.pop();

            return true;
        }

        //-----------------------------------//

        void wait_and_pop(T& popped_value)
        {
            std::unique_lock<std::mutex> lock{ _mutex };

            while (queue.empty())
                _cond.wait(lock);

            popped_value = queue.front();
            queue.pop();
        }

        //-----------------------------------//

    public:
        std::mutex _mutex;
        std::condition_variable _cond;

        std::queue<T> queue;
    };

    //-----------------------------------//

}