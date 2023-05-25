#include "FutureCoro.h"
#include "FutureAwaiter.h"
#include "MemoryLeakDetector.h"
#include "CoroExamples.h" 

#include "gtest/gtest.h"

TEST(Coroutines, TaskDoesntStartUntilAwaited)
{
    MemoryLeakDetector d;
	bool started = false;
	auto func = [](bool& started) -> FutureCoro<>
	{
		started = true;
		co_return;
	}(started);

	[](bool& started, FutureCoro<>&& func) -> FutureCoro<>
	{
		EXPECT_FALSE(started);
		co_await std::move(func);
		EXPECT_TRUE(started);
        co_return;
	}(started, std::move(func)).get();
}

TEST(Coroutines, TaskDoesNotStackOverflow)
{
    MemoryLeakDetector d;
    ASSERT_NO_THROW({
        loop_asynchronously(100).get();
        loop_asynchronously(1000).get();
        // loop_asynchronously(100'000).get();
        // loop_asynchronously(1'000'000).get();
        // is commented out because takes 86s to run :(
    });
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
    ASSERT_THROW({ throwsTask().get(); },
        my_exception);
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