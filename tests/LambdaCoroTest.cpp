#include "FutureCoro.h"
#include "FutureAwaiter.h"
#include "DtorCounter.h"
#include "LambdaCoro.h"
#include "MemoryLeakDetector.h"

#include "gtest/gtest.h"
#include <chrono>
#include <thread>

TEST(LambdaCoroutines, CapturingByValueCanLeak)
{
    MemoryLeakDetector d;

    {
        auto l1 = capturesByValue();
        auto result = l1.get();
        EXPECT_NE(result.state, 5);
    }

    EXPECT_EQ(DtorCounter::leaked(), 0);
}

TEST(LambdaCoroutines, CapturingByRefCanLeak)
{
    MemoryLeakDetector d;

    {
        DtorCounter original;
        auto l1 = capturesByRef(original);
        EXPECT_THROW({l1.get();}, nullptr_exception);
    }

    EXPECT_EQ(DtorCounter::leaked(), 0);
}

TEST(LambdaCoroutines, TestCase2)
{
    auto l1 = get_fake_lambda();
    EXPECT_EQ(l1.get(), 5);
    auto l2 = get_real_lambda();
    EXPECT_NE(l2.get(), 6); // use-after-free
}