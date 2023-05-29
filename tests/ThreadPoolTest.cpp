#include "ThreadPool.h"
#include "util.h"

#include "gtest/gtest.h"
#include <chrono>

auto simpleBenchmark(thread_pool &threadPool)
{
    std::atomic_int i = 0;
    const int nTasks = 25;
    auto task = [&i]()
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2ms);
        i++;
    };

    auto startTime = std::chrono::high_resolution_clock::now();
    for (auto j = 0; j < nTasks; j++)
    {
        threadPool.submit(task);
    }

    while (i.load() != nTasks);
    return elapsedTimeNano(startTime);
}

TEST(ThreadPool, ThreadpoolWithMoreThreadsIsQuickier)
{
    thread_pool p1{1};
    thread_pool p2{2};
    thread_pool p4{4};
    thread_pool p8{8};

    auto t1 = simpleBenchmark(p1);
    auto t2 = simpleBenchmark(p2);
    auto t4 = simpleBenchmark(p4);
    auto t8 = simpleBenchmark(p8);

    EXPECT_GT(t1, t2);
    EXPECT_GT(t2, t4);
    EXPECT_GT(t4, t8);
}