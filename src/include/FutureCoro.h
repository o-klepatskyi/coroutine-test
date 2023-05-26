#pragma once
#include "log.h"

#include <future>
#include <type_traits>
#include <coroutine>
#include <thread>

struct async_start_awaiter
{
    bool await_ready() const noexcept { return false; }

    bool await_suspend(std::coroutine_handle<> handle) const noexcept {
        std::thread ([handle](){ handle.resume(); }).detach();
        return true;
    }

    void await_resume() const noexcept {}
};

struct async_final_awaiter
{
    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) const noexcept {
        handle.destroy();
    }

    void await_resume() const noexcept {}
};

template <typename T = void>
requires(!std::is_reference_v<T>)
class FutureCoro
{
public:
    struct promise_type : std::promise<T>
    {

        FutureCoro<T> get_return_object() noexcept
        {
            return { std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        auto initial_suspend() const noexcept
        {
            return async_start_awaiter {};
        }

        auto final_suspend() const noexcept
        {
            return async_final_awaiter {};
        }

        void return_value(T&& value)
        noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            this->set_value(std::move(value));
        }

        void unhandled_exception() noexcept
        {
            this->set_exception(std::current_exception());
        }

    };

    using CoroHandle = std::coroutine_handle<promise_type>;
protected:
    CoroHandle handle;
    std::future<T> fut;
public:

    FutureCoro(CoroHandle h) noexcept
        : handle(h), fut(h.promise().get_future())
    {}

    FutureCoro(const FutureCoro&) = delete;
    FutureCoro& operator=(const FutureCoro&) = delete;

    FutureCoro(FutureCoro&& other) noexcept
        : handle(other.handle), fut(std::move(other.fut))
    {
        other.handle = nullptr;   
    }

    T get()
    {
        return fut.get();
    }

    void wait() const
    {
        fut.wait();
    }

    bool ready() const
    {
        using namespace std::chrono_literals;
        return fut.wait_for(0s) == std::future_status::ready;
    }
};

template <>
class FutureCoro<void>
{
public:
    struct promise_type : std::promise<void>
    {

        FutureCoro<void> get_return_object() noexcept
        {
            return { std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        auto initial_suspend() const noexcept { return async_start_awaiter {}; }

        auto final_suspend() const noexcept { return async_final_awaiter {}; }

        void return_void() noexcept
        {
            this->set_value();
        }

        void unhandled_exception() noexcept
        {
            this->set_exception(std::current_exception());
        }
    };

    using CoroHandle = std::coroutine_handle<promise_type>;
protected:
    CoroHandle handle;
    std::future<void> fut;
public:

    FutureCoro<void>(CoroHandle h) noexcept
        : handle(h), fut(h.promise().get_future())
    {}

    FutureCoro<void>(const FutureCoro<void>&) = delete;
    FutureCoro<void>& operator=(const FutureCoro<void>&) = delete;

    FutureCoro<void>(FutureCoro<void>&& other) noexcept
        : handle(other.handle), fut(std::move(other.fut))
    {
        other.handle = nullptr;   
    }

    void get()
    {
        return fut.get();
    }

    void wait() const
    {
        fut.wait();
    }

    bool ready() const
    {
        using namespace std::chrono_literals;
        return fut.wait_for(0s) == std::future_status::ready;
    }
};