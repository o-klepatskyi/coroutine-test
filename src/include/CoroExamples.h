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

FutureCoro<std::vector<int>> destructor_sequence_coro()
{
    std::vector<int> destructor_ids;
    using namespace std::chrono_literals;

    struct destructor_recorder
    {
        std::vector<int>& results;
        const int id;
        ~destructor_recorder() noexcept { results.push_back(id); }
    };

    co_await [&destructor_ids]() -> FutureCoro<void>
    {
        destructor_recorder dr {destructor_ids, 1};
        std::this_thread::sleep_for(10ms);
        co_return;
    }();

    EXPECT_EQ(destructor_ids.size(), 1u);
    EXPECT_EQ(destructor_ids[0], 1);

    co_await [&destructor_ids]() -> FutureCoro<void>
    {
        destructor_recorder dr {destructor_ids, 2};
        std::this_thread::sleep_for(5ms);
        co_return;
    }();

    EXPECT_EQ(destructor_ids.size(), 2u);
    EXPECT_EQ(destructor_ids[1], 2);

    co_await [&destructor_ids]() -> FutureCoro<void>
    {
        destructor_recorder dr {destructor_ids, 3};
        co_return;
    }();

    EXPECT_EQ(destructor_ids.size(), 3u);
    EXPECT_EQ(destructor_ids[2], 3);

    co_return destructor_ids;
}

// silly implementation
FutureCoro<int> fibCoro(int n)
{
    if (n <= 1)
        co_return n;
    
    auto f1 = fibCoro(n - 1);
    auto f2 = fibCoro(n - 2);

    co_return co_await std::move(f1) + co_await std::move(f2);
}
