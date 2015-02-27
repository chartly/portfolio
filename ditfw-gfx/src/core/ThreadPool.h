/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread.hpp>

#include <functional>
#include <queue>

namespace dit {

    class ThreadPool
    {
    public:
        typedef std::function<void()> Runnable;

    private:
        std::size_t available_ = 0;
        bool running_ = true;

        std::queue<Runnable> tasks_;
        boost::thread_group threads_;
        boost::mutex mutex_;
        boost::condition_variable condition_;

    public:

        ThreadPool(std::size_t pool_size)
            : available_(pool_size)
            , running_(true)
        {
            for (std::size_t i = 0; i < pool_size; ++i)
            {
                threads_.create_thread([=]() {
                    this->_ThreadMain();
                });
            }
        }

        ~ThreadPool()
        {
            {
                boost::unique_lock< boost::mutex > lock(mutex_);
                running_ = false;
                condition_.notify_all();
            }

            try
            {
                threads_.join_all();
            }
            catch (...) // HACK: <------ :/
            {
                // ...
            }
        }

        ThreadPool(const ThreadPool& rhs) = delete;
        ThreadPool& operator=(const ThreadPool& rhs) = delete;

    public:

        template < typename Task >
        void Post(Task task)
        {
            boost::unique_lock< boost::mutex > lock(mutex_);

            // if no threads are available, then dip out.
            if (available_ == 0)
                return;

            // decrement count, indicating thread is no longer available.
            --available_;

            // set task and signal condition variable so that a worker thread will wake up and use the task.
            tasks_.push(Runnable{ task });
            condition_.notify_one();
        }

    public:
        // entry point for pool threads.
        void _ThreadMain()
        {
            while (running_)
            {
                // wait on condition variable while the task is empty and the pool is still running.
                boost::unique_lock< boost::mutex > lock(mutex_);
                while (tasks_.empty() && running_)
                    condition_.wait(lock);

                // if pool is no longer running, break out.
                if (!running_)
                    break;

                // copy task locally and remove from the queue.  This is done within
                // its own scope so that the task object is destructed immediately
                // after running the task.  This is useful in the event that the
                // function contains shared_ptr arguments bound via bind.
                {
                    Runnable task = tasks_.front();
                    tasks_.pop();

                    lock.unlock();

                    try
                    {
                        task();
                    }
                    catch (...) // HACK: <------ :/
                    {
                        // ...
                    }
                }

                // task has finished, so increment count of available threads.
                lock.lock();
                ++available_;
            }
        }
    };

}