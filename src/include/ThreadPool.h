#pragma once

#include <thread>
#include <functional> // std::function
#include <concepts>

class thread_pool final
{
public:

    template<typename... Args, std::invocable<Args...> TaskT>
    static void start(TaskT task, Args&&... args) noexcept
    {
        // just spawns a new thread
        // TODO: reuse threads
        std::thread (
            std::move(task), std::forward<Args>(args)...
        ).detach();
    }
};