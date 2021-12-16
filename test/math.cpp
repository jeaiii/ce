#include "ce/math.h"

#include "gtest/gtest.h"

GTEST_TEST(math, ab_less_cd)
{
    ce::int32_t a = 5;
    ce::int32_t b = 7;
    ce::int32_t c = 11;
    ce::int32_t d = 13;

    GTEST_EXPECT_TRUE(ce::ab_less_cd(a, b, c, d));
}

GTEST_TEST(math, inside)
{
    ce::vec2<int32_t> v00{ 0, 0 };
    ce::vec2<int32_t> v10{ 10, 0 };
    ce::vec2<int32_t> v01{ 0, 10 };
    ce::vec2<int32_t> v11{ 10, 10 };

    GTEST_EXPECT_TRUE(ce::inside(v01, v10));
    GTEST_EXPECT_TRUE(ce::inside(v00, v10));
    GTEST_EXPECT_TRUE(!ce::inside(v10, v01));
    GTEST_EXPECT_TRUE(!ce::inside(v10, v00));
}
