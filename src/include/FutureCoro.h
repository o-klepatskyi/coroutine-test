#pragma once
#include <future>
#include <type_traits>
#include <coroutine>
#include <thread>

template <typename T>
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

        std::suspend_always initial_suspend() const noexcept { return {}; }
        std::suspend_always final_suspend() const noexcept { return {}; }

        void return_value(const T& value)
        noexcept(std::is_nothrow_copy_constructible_v<T>)
        {
            this->set_value(value);
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
private:
    CoroHandle handle;
    std::future<T> fut;
public:

    FutureCoro(CoroHandle h) noexcept
        : handle(h), fut(h.promise().get_future())
    {}

    ~FutureCoro()
    {
        if (handle)
        {
            handle.destroy();
        }
    }

    FutureCoro(const FutureCoro&) = delete;
    FutureCoro& operator=(const FutureCoro&) = delete;

    FutureCoro(FutureCoro&& other) noexcept
        : handle(other.handle), fut(std::move(other.fut))
    {
        other.handle = nullptr;   
    }

    T get() { return fut.get(); }

    void wait() const
    {
        std::thread( [this]() mutable { this->handle.resume(); }).detach();
        fut.wait();
    }

    bool ready() const
    {
        using namespace std::chrono_literals;
        return fut.wait_for(0s) == std::future_status::ready;
    }
protected:
    void resume() const
    {
        handle.resume();
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

        std::suspend_always initial_suspend() const noexcept { return {}; }
        std::suspend_always final_suspend() const noexcept { return {}; }

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
private:
    CoroHandle handle;
    std::future<void> fut;
public:

    FutureCoro<void>(CoroHandle h) noexcept
        : handle(h), fut(h.promise().get_future())
    {}

    ~FutureCoro()
    {
        if (handle)
        {
            handle.destroy();
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
        fut.get();
    }

    void wait() const
    {
        std::thread( [this]() mutable { this->handle.resume(); }).detach();
        fut.wait();
    }

    bool ready() const
    {
        using namespace std::chrono_literals;
        return fut.wait_for(0s) == std::future_status::ready;
    }
protected:
    void resume() const
    {
        handle.resume();
    }
};