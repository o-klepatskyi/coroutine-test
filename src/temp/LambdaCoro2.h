#pragma once

#include <coroutine>
#include <functional>
#include <optional>
#include <cassert>

template <typename T>
class lazy {
    std::function<T ()> _compute;
    lazy(std::function<T ()> compute) : _compute(std::move(compute)) {}
public:
    T get() { return _compute(); }
    static lazy make(std::function<T ()> compute) {
        return lazy(std::move(compute));
    }
};

namespace std {

template <typename T, typename... Args>
struct coroutine_traits<lazy<T>, Args...> {
    struct promise_type {
        std::optional<T> value;
        suspend_always initial_suspend() const noexcept { return {}; }
        suspend_always final_suspend() const noexcept { return {}; }

        void return_value(T val) {
            value = std::move(val);
        }
        lazy<T> get_return_object() {
            return lazy<T>::make([this] () -> T {
                auto handle = coroutine_handle<promise_type>::from_promise(*this);
                handle.resume();
                auto ret = std::move(*value);
                handle.destroy();
                return ret;
            });
        }
        void unhandled_exception() {
            std::terminate();
        }
    };
};

}

struct fake_lambda_state {
    int i = 5;
};

lazy<int> fake_lambda(fake_lambda_state s) {
    co_return s.i;
}

lazy<int> get_fake_lambda() {
    // fake_lambda_state() is captured in the promise
    return fake_lambda(fake_lambda_state());
}

lazy<int> get_real_lambda() {
    // the state (i) is not captured
    return [i = 6] () -> lazy<int> {
        co_return i;
    }();
}
