#pragma once

#include <coroutine>
#include <future>
#include <exception> // std::current_exception
#include <memory>
#include <cassert>

/**
 * @brief Allows awaiting on a lambda
 * TODO: Add lambda arguments
 */
template<class Task>
struct lambda_awaiter
{
    Task action;
    using T = decltype(action());
    std::unique_ptr<std::future<T>> result;

    explicit lambda_awaiter(Task&& task) noexcept : action{ std::move(task) } {}

    // is the task ready?
    bool await_ready() const noexcept
    {
        if (!result) // task hasn't even been created yet!
        {
            return false;
        }
        using namespace std::chrono_literals;
        return result->wait_for(0s) != std::future_status::timeout;
    }

    // suspension point that launches the background async task
    void await_suspend(std::coroutine_handle<> cont) noexcept
    {
        if (result) std::terminate(); // avoid task explosion
        std::promise<T> p;
        result = std::make_unique<std::future<T>>(p.get_future());

        std::thread t([this, cont] (std::promise<T>&& promise)
        {
            try
            {
                promise.set_value(action());
            }
            catch(...)
            {
                promise.set_exception(std::current_exception());
            }
            cont.resume();
        }, std::move(p));
        t.detach();
    }

    // resume & get the value once `cont()` is signaled
    T await_resume() noexcept
    {
        assert(result);
        return result->get(); // TODO: fix bug with exceptions
    }
};

/**
 * @brief Allow co_await'ing any invocable functors via rpp::async_task
 */
template<typename Task>
    requires std::is_invocable_v<Task>
lambda_awaiter<Task> operator co_await(Task&& task) noexcept
{
    return lambda_awaiter<Task>{ std::move(task) };
}