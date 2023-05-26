#pragma once
#include "FutureCoro.h"

#include <coroutine>

template<typename T>
struct awaiter final : public FutureCoro<T>
{
    bool await_ready() const noexcept
    {
        return this->ready();
    }

    bool await_suspend(std::coroutine_handle<>) noexcept
    {
        return false;
    }

    T await_resume()
    {
        return this->fut.get();
    }
};

template<>
struct awaiter<void> : public FutureCoro<void>
{
public:

    bool await_ready() const noexcept
    {
        return this->ready();
    }

    bool await_suspend(std::coroutine_handle<>) noexcept
    {
        return false;
    }

    void await_resume()
    {
        this->fut.get();
    }
};

template<typename T>
auto operator co_await(FutureCoro<T>&& future) noexcept
{
    return awaiter<T> { std::move(future) };
}