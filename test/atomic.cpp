#include "ce/atomic_msvc_x86.h"

#include "gtest/gtest.h"

ce::atomic<int> ai;
ce::atomic<float> af;
ce::atomic<int*> ap;

ce::atomic<int> ceii{ 5 };
std::atomic<int> stdi{ 5 };

GTEST_TEST(atomic, atomic)
{

    //ceii = ai;

    ce::atomic_store(ai, 0);

    GTEST_EXPECT_TRUE(ce::atomic_fetch_add(ai, 5) == 0);
    GTEST_EXPECT_TRUE(ce::atomic_fetch_sub(ai, 2) == 5);

    int n = 10;

#if 0
    GTEST_EXPECT_TRUE(ce::atomic_fetch_xor(ai, 1) == 3);
    GTEST_EXPECT_TRUE(ce::atomic_fetch_and(ai, 0) == 2);
    GTEST_EXPECT_TRUE(ce::atomic_fetch_or(ai, 7) == 0);
#endif
    ce::atomic_store(ai, 7);

    GTEST_EXPECT_TRUE(ce::atomic_exchange(ai, 3) == 7);

    GTEST_EXPECT_TRUE(ce::atomic_compare_exchange(ai, n, 9) == false);
    GTEST_EXPECT_TRUE(n == 3);
    GTEST_EXPECT_TRUE(ce::atomic_compare_exchange(ai, n, 9) == true);
    GTEST_EXPECT_TRUE(n == 3);

//    ce::atomic_store(ai, 9);

    GTEST_EXPECT_TRUE(ce::atomic_load(ai) == 9);

    ce::atomic_fetch_add(af, 10);
    ce::atomic_fetch_sub(af, 5);
    ce::atomic_exchange(af, 7);
    float g = 7.0f;
    ce::atomic_compare_exchange(af, g, 8.0f);
    GTEST_EXPECT_TRUE(ce::atomic_load(af) == 8.0f);

    ce::atomic_store(ap, &n);
    ce::atomic_fetch_add(ap, 10);

    auto const& ar(ap);

    GTEST_EXPECT_TRUE(ce::atomic_load(ar) == &n + 10);

    ce::atomic_fetch_add(ap, 10);
    GTEST_EXPECT_TRUE(ce::atomic_load(ar) == &n + 20);
}