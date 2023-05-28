#pragma once
#include <coroutine>

template <typename T>
struct task {
};

namespace std {

template <typename T, typename Lambda, typename... MoreArgs>
class coroutine_traits<task<T>, Lambda, MoreArgs...> {
// public:
    struct promise_type {
    };
};

template <typename T, typename Lambda, typename... MoreArgs>
class coroutine_traits<task<T>, Lambda*, MoreArgs...> {
    // declared private to show that the pointer version of the traits is
    // selected
public:
    struct promise_type {
    };
};

}

template <typename Lambda>
void with_coroutine(Lambda lambda) {
}

void foo()
{
    with_coroutine([i = 3] () -> task<int> {
        co_return i;
    });
}