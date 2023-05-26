#pragma once
#include "log.h"

#include <future>
#include <type_traits>
#include <coroutine>
#include <thread>

struct start_awaiter
{
    bool await_ready() const noexcept { return false; }

    bool await_suspend(std::coroutine_handle<> handle) const noexcept {
        std::thread ([handle](){ handle.resume(); }).detach();
        return true;
    }

    void await_resume() const noexcept {}
};

struct final_awaiter
{
    bool await_ready() const noexcept { return false; }

    bool await_suspend(std::coroutine_handle<> handle) const noexcept {
        handle.destroy();
        return true;
    }

    void await_resume() const noexcept
    {

    }
};

template <typename T = void>
requires(!std::is_reference_v<T>)
class FutureCoro
{
public:
    struct promise_type
    {
        std::promise<T> promise;
        std::coroutine_handle<promise_type> promiseHandle {};

        FutureCoro<T> get_return_object() noexcept
        {
            promiseHandle = std::coroutine_handle<promise_type>::from_promise(*this);
            return { promiseHandle };
        }

        ~promise_type() noexcept
        {
            LogInfo("~promise_type()");
        }

        auto initial_suspend() const noexcept
        {
            return start_awaiter {};
        }

        auto final_suspend() noexcept
        {
            LogInfo("final_suspend");
            destroyme();
            return std::suspend_always {};
        }

        void return_value(T&& value)
        noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            LogInfo("return_value <T>");
            promise.set_value(std::move(value));
            //destroyme();
        }

        void unhandled_exception() noexcept
        {
            LogInfo("unhandled_exception <T>");
            promise.set_exception(std::current_exception());
            //destroyme();
        }

        void destroyme() noexcept
        {
            if (promiseHandle) promiseHandle.destroy();
        }
    };

    using CoroHandle = std::coroutine_handle<promise_type>;
protected:
    CoroHandle handle;
    std::future<T> fut;
public:

    FutureCoro(CoroHandle h) noexcept
        : handle(h), fut(h.promise().promise.get_future())
    {
        LogInfo("FutureCoro<T>()");
    }

    ~FutureCoro()
    {
        if (handle)
        {
            LogInfo("WAITING ~FutureCoro<T>()");
            //fut.wait();
            //handle.destroy();
        }
    }

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
    struct promise_type
    {
        std::promise<void> promise;
        std::coroutine_handle<promise_type> promiseHandle {};

        FutureCoro<void> get_return_object() noexcept
        {
            promiseHandle = std::coroutine_handle<promise_type>::from_promise(*this);
            return { promiseHandle };
        }

        ~promise_type() noexcept
        {
            LogInfo("~promise_type()");
        }

        start_awaiter initial_suspend() const noexcept { return {}; }

        auto final_suspend() noexcept
        {
            LogInfo("final_suspend");
            destroyme();
            return std::suspend_always {};
        }

        void return_void() noexcept
        {
            LogInfo("return_void");
            promise.set_value();
            //destroyme();
        }

        void unhandled_exception() noexcept
        {
            LogInfo("unhandled_exception <void>");
            promise.set_exception(std::current_exception());
            //destroyme();
        }

        void destroyme() noexcept
        {
            if (promiseHandle) promiseHandle.destroy();
        }
    };

    using CoroHandle = std::coroutine_handle<promise_type>;
protected:
    CoroHandle handle;
    std::future<void> fut;
public:

    FutureCoro<void>(CoroHandle h) noexcept
        : handle(h), fut(h.promise().promise.get_future())
    {
        LogInfo("FutureCoro<void>()");
    }

    ~FutureCoro()
    {
        if (handle)
        {
            LogInfo("WAITING ~FutureCoro<void>()");
            //fut.wait();
            //handle.destroy();
        }
    }

    FutureCoro<void>(const FutureCoro<void>&) = delete;
    FutureCoro<void>& operator=(const FutureCoro<void>&) = delete;

    FutureCoro<void>(FutureCoro<void>&& other) noexcept
        : handle(other.handle), fut(std::move(other.fut))
    {
        other.handle = nullptr;   
    }

    void get()
    {
        // there are a lot of bugs that can appear if we do it this way
        // with std::suspend_always in initial_suspend
        // if (ready()) return fut.get(); 
        // std::thread( [this]() mutable { handle.resume(); }).detach();
        return fut.get();
    }

    void wait() const
    {
        // if (ready()) return;
        // std::thread( [this]() mutable { this->handle.resume(); }).detach();
        fut.wait();
    }

    bool ready() const
    {
        using namespace std::chrono_literals;
        return fut.wait_for(0s) == std::future_status::ready;
    }
};