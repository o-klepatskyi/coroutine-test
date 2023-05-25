#pragma once
#include "FutureCoro.h"
#include "FutureAwaiter.h"

#include <iostream>
#include <exception>

FutureCoro<> completes_asynchronously() {
    co_return;
}

FutureCoro<> loop_asynchronously(int count) {
    std::cout << "loop_asynchronously(" << count << ")\n";
    for (int i = 0; i < count; ++i) {
        co_await completes_asynchronously();
    }
    std::cout << "loop_asynchronously(" << count << ") returning\n";
}

struct my_exception : public std::exception {};