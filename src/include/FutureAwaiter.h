#pragma once
#include "FutureCoro.h"

#include <future>
#include <thread>
#include <coroutine>


template<typename T>
struct awaiter : FutureCoro<T>
{
    bool await_ready() const noexcept
    {
        return this->ready();
    }

    void await_suspend(std::coroutine_handle<> cont) noexcept
    {
        std::thread(
            [this, cont] () mutable { 
                this->resume();
                cont.resume();
            }
        ).detach();
    }

    T await_resume()
    {
        return this->get();
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

    void await_suspend(std::coroutine_handle<> cont) noexcept
    {
        std::thread(
            [this, cont] () mutable {
                this->resume();
                cont.resume();
            }
        ).detach();
    }

    void await_resume()
    {
        this->get();
    }
};

template<typename T>
auto operator co_await(FutureCoro<T> future) noexcept
{
    return awaiter<T> { std::move(future) };
}