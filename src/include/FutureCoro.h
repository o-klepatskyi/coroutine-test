#pragma once
#include "ThreadPool.h"
#include "log.h"
#include <future>
#include <type_traits>
#include <coroutine>

struct async_start_awaiter
{
    bool await_ready() const noexcept { return false; }

    bool await_suspend(std::coroutine_handle<> handle) const noexcept {
        thread_pool::start([handle]() {
            handle.resume();
            LogInfo("finished execution!");
        });
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
        std::exception_ptr ex = nullptr;
        T result {};

        FutureCoro<T> get_return_object() noexcept
        {
            return { std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        auto initial_suspend() const noexcept
        {
            return async_start_awaiter {};
        }

        auto final_suspend() noexcept
        {
            LogInfo("final suspend...");
            if (ex)
            {
                this->set_exception(ex);
            } else
            {
                this->set_value(result);
            }
            return std::suspend_always {};
        }

        void return_value(const T& value)
#if _MSC_VER
        noexcept(std::is_nothrow_copy_assignable<T>)
#endif
        {
            result = value;
        }

        void return_value(T&& value)
        noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            result = std::move(value);
        }

        void unhandled_exception() noexcept
        {
            ex = std::current_exception();
        }

    };

    using CoroHandle = std::coroutine_handle<promise_type>;
protected:
    CoroHandle handle;
    std::shared_future<T> fut;
public:

    FutureCoro(CoroHandle h) noexcept
        : handle(h), fut(h.promise().get_future().share())
    {}

    FutureCoro(const FutureCoro&) = delete;
    FutureCoro& operator=(const FutureCoro&) = delete;

    FutureCoro(FutureCoro&& other) noexcept
        : handle(other.handle), fut(std::move(other.fut))
    {
        other.handle = nullptr;
    }

    ~FutureCoro() noexcept
    {
        if (handle && fut.valid())
        {
            LogInfo("destroying...");
            fut.wait();
            handle.destroy();
        }
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
        std::exception_ptr ex = nullptr;

        FutureCoro<void> get_return_object() noexcept
        {
            return { std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        auto initial_suspend() const noexcept { return async_start_awaiter {}; }

        auto final_suspend() noexcept
        {
            LogInfo("final suspend...");
            if (ex)
            {
                this->set_exception(ex);
            } else
            {
                this->set_value();
            }
            return std::suspend_always {};
        }

        void return_void() noexcept
        {
            
        }

        void unhandled_exception() noexcept
        {
            ex = std::current_exception();
        }
    };

    using CoroHandle = std::coroutine_handle<promise_type>;
protected:
    CoroHandle handle;
    std::shared_future<void> fut;
public:

    FutureCoro(CoroHandle h) noexcept
        : handle(h), fut(h.promise().get_future().share())
    {}

    FutureCoro(const FutureCoro&) = delete;
    FutureCoro& operator=(const FutureCoro&) = delete;

    FutureCoro(FutureCoro&& other) noexcept
        : handle(other.handle), fut(std::move(other.fut))
    {
        other.handle = nullptr;   
    }

    ~FutureCoro() noexcept
    {
        if (handle && fut.valid())
        {
            LogInfo("destroying...");
            fut.wait();
            handle.destroy();
        }
    }

    void get()
    {
        fut.get();
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