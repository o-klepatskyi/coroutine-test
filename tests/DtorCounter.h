#pragma once
#include <mutex>

class DtorCounter
{
    static int n_destroyed;
    static int n_created;
    static std::mutex mut;
    using lg = std::lock_guard<std::mutex>;
public:
    DtorCounter() noexcept
    {
        lg l(mut);
        n_created++;
    }
    DtorCounter(const DtorCounter&) noexcept
    {
        lg l(mut);
        n_created++;
    }
    DtorCounter& operator=(const DtorCounter&) = default;
    DtorCounter(DtorCounter&&) noexcept
    {
        lg l(mut);
        n_created++;
    }
    DtorCounter& operator=(DtorCounter&&) = default;

    ~DtorCounter() noexcept
    {
        lg l(mut);
        n_destroyed++;
    }

    static int leaked() noexcept
    {
        lg l(mut);
        return n_created - n_destroyed;
    }
};