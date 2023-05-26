#pragma once
#include "FutureCoro.h"

#include <future>
#include <thread>
#include <coroutine>

template<typename T>
struct awaiter final : public FutureCoro<T>
{
    bool await_ready() const noexcept
    {
        LogInfo("await_ready");
        return this->ready();
    }

    bool await_suspend(std::coroutine_handle<> cont) noexcept
    {
        // std::thread(
        //     [this, cont] () mutable { 
        //         this->handle.resume();
        //         cont.resume();
        //     }
        // ).detach();
        LogInfo("await_suspend");
        return false;
    }

    T await_resume()
    {
        LogInfo("WAITING await_resume");
        return this->fut.get();
    }
};

template<>
struct awaiter<void> final : public FutureCoro<void>
{
public:

    bool await_ready() const noexcept
    {
        LogInfo("await_ready");
        return this->ready();
    }

    bool await_suspend(std::coroutine_handle<> cont) noexcept
    {
        // std::thread(
        //     [this, cont] () mutable {
        //         this->handle.resume();
        //         cont.resume();
        //     }
        // ).detach();
        LogInfo("await_suspend");
        return false;
    }

    void await_resume()
    {
        LogInfo("WAITING await_resume");
        this->fut.get();
    }
};

template<typename T>
auto operator co_await(FutureCoro<T>&& future) noexcept
{
    LogInfo("co_await");
    return awaiter<T> { std::move(future) };
}