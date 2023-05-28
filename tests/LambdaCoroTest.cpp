#include "FutureCoro.h"
#include "FutureAwaiter.h"
#include "DtorCounter.h"
// #include "LambdaCoro.h"
// #include "LambdaCoro2.h"


#include "gtest/gtest.h"
#include <chrono>
#include <thread>

// wasn't able to replicate this bug yet (used gcc to build)
// clang compiles it without lambda pointer param, maybe it is also
// the case with gcc now?
// https://cppinsights.io/s/c5ac0ad7

TEST(LambdaCoroutines, CoroutineLambdaLeaksMemory)
{
    {
        DtorCounter c;

        auto l2 = []() -> FutureCoro<void> { co_return; };

        auto l1 = [c, l2]() -> FutureCoro<DtorCounter> {
            co_await l2();
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10ms);
            co_return c;
        };

        {
            l1(); // starts asynchronously
        } // l1 dtor is called here
    }

    EXPECT_EQ(DtorCounter::leaked(), 1);
}

// TEST(LambdaCoroutines, TestCase1)
// {
//     foo();
// }

// TEST(LambdaCoroutines, TestCase2)
// {
//     auto l1 = get_fake_lambda();
//     EXPECT_EQ(l1.get(), 5);
//     auto l2 = get_real_lambda();
//     EXPECT_EQ(l2.get(), 6);
// }