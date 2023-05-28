#include "FutureCoro.h"
#include "FutureAwaiter.h"
#include "MemoryLeakDetector.h"
#include "CoroExamples.h"

#include "gtest/gtest.h"
#include <vector>
#include <string>
#include <cassert>

TEST(Coroutines, CoroStartsOnCreationAndAsyncsOnAwait)
{
    MemoryLeakDetector d;
	bool started = false;
	auto func = [&]() -> FutureCoro<>
	{
		started = true;
		co_return;
	};

	[&]() -> FutureCoro<>
	{
		EXPECT_FALSE(started);
		co_await func();
		EXPECT_TRUE(started);
        co_return;
	}().get();
}

TEST(Coroutines, TaskReturnsCorrectly)
{
    MemoryLeakDetector d;
    const int i = 15;
    auto doubleTask = [] (int x) -> FutureCoro<int> {
        co_return x * 2;
    };

    auto userTask = [&doubleTask]() -> FutureCoro<int> {
        co_return co_await doubleTask(15);
    };

    auto result = userTask().get();

    ASSERT_EQ(i * 2, result);
}

TEST(Coroutines, VoidTaskThrowsOnExecution)
{
    MemoryLeakDetector d;
    auto throwsTask = []() -> FutureCoro<> {
        throw my_exception {};
    };
    ASSERT_THROW({ throwsTask().get(); }, my_exception);
}

TEST(Coroutines, NonVoidTaskThrowsOnObtainingResult)
{
    MemoryLeakDetector d;
    auto throwsTask = []() -> FutureCoro<int> {
        throw my_exception {};
        co_return 0;    // if we don't provide co_return,
                        // it throws with another exception
    };

    ASSERT_THROW(throwsTask().get(), my_exception);
}

TEST(Coroutines, RunsInAnotherThread)
{
    MemoryLeakDetector d;
    std::vector<std::thread::id> ids;
    ids.reserve(10);
    ids.emplace_back(std::this_thread::get_id()); // 0
    auto co1 = [&]() -> FutureCoro<> {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
        ids.emplace_back(std::this_thread::get_id()); // 2, 4
        co_return;  
    };

    auto co2 = [&]() -> FutureCoro<> {
        ids.emplace_back(std::this_thread::get_id()); // 1
        co_await co1();
        ids.emplace_back(std::this_thread::get_id()); // 3
        co_await co1();
        ids.emplace_back(std::this_thread::get_id()); // 5
        co_return;
    };
    co2().wait();
    ASSERT_EQ(ids.size(), 6); 
    EXPECT_TRUE(ids[0] != ids[1]); // starts coro in another thread
    EXPECT_TRUE(ids[1] != ids[2]); // co_await starts asynchronously
    EXPECT_TRUE(ids[1] == ids[3]); // main thread waits and continues
    EXPECT_TRUE(ids[3] != ids[4]); // start async again 
    EXPECT_TRUE(ids[3] == ids[5]); // waits and continues again 
}

TEST(Coroutines, CorrectlyAccumulatesAndReturnsValue)
{
    MemoryLeakDetector d;
    auto co1 = [](std::string s) -> FutureCoro<std::string> {
        co_return s + "123";
    };
    auto co2 = [&](std::string s) -> FutureCoro<std::string> {
        auto s1 = co_await co1(s);
        co_return s1 + "456";
    };
    auto co3 = [&]() -> FutureCoro<std::string> {
        auto s1 = co_await co2("");
        co_return s1 + "789";
    };
    auto result = co3().get();
    EXPECT_STREQ(result.c_str(), "123456789");
}

TEST(Coroutines, MemoryIsFreedIfCoroutineIsNotAwaited)
{
    using namespace std::chrono_literals;
    MemoryLeakDetector d;
    auto co1 = []() -> FutureCoro<> {
        std::vector<int> vec;
        vec.reserve(100000); // allocating a lot of memory
        co_return;
    };
    co1();
}

TEST(Coroutines, MemoryIsFreedAfterException)
{
    using namespace std::chrono_literals;
    MemoryLeakDetector d;
    auto co1 = []() -> FutureCoro<> {
        std::vector<int> vec;
        vec.reserve(100000); // allocating a lot of memory
        throw std::exception {};
        co_return;
    };
    co1();
}

TEST(Coroutines, ensure_destructors_are_called_sequentially)
{
    MemoryLeakDetector d;
    std::vector<int> destructor_ids = destructor_sequence_coro().get();
    ASSERT_EQ(destructor_ids.size(), 3u);
    ASSERT_EQ(destructor_ids[0], 1);
    ASSERT_EQ(destructor_ids[1], 2);
    ASSERT_EQ(destructor_ids[2], 3);
}

TEST(Coroutines, MemoryIsFreedWithRecursion)
{
    GTEST_SKIP(); // does not work with thread pool
    MemoryLeakDetector d;
    EXPECT_EQ(fibCoro(10).get(), 55);
    // with bigger numbers system_error is thrown
    // I assume there are no more threads awailable
    // fibCoro(14).get();
}

TEST(Coroutines, CorrectlyModifiesByRef)
{
    std::string str = "coro";
    auto modify = [](std::string& str) -> FutureCoro<void> {
        str += "utine";
        co_return;
    }(str);
    EXPECT_NO_THROW({ modify.get(); });
    EXPECT_STREQ("coroutine", str.c_str());
}