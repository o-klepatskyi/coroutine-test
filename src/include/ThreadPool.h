#pragma once
#include "ThreadSafeQueue.h"

#include <concepts>
#include <functional> // std::function
#include <thread>

// @brief Trivial fixed thread pool implementation
class thread_pool
{
    std::atomic_bool done;
    threadsafe_queue<std::function<void()>> work_queue;
    std::vector<std::jthread> threads;

    void worker_thread()
    {
        while (!done)
        {
            std::function<void()> task;
            if (work_queue.try_pop(task))
            {
                task();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

public:
    thread_pool(unsigned int const thread_count = std::thread::hardware_concurrency()) : done(false)
    {
        try
        {
            for (unsigned i = 0; i < thread_count; ++i)
            {
                threads.push_back(
                    std::jthread(&thread_pool::worker_thread, this));
            }
        }
        catch (...)
        {
            done = true;
            throw;
        }
    }

    ~thread_pool() noexcept
    {
        done = true;
    }

    template <typename FunctionType>
    void submit(FunctionType f) noexcept
    {
        work_queue.push(std::function<void()>(f));
    }
};

static thread_pool GlobalThreadPool;