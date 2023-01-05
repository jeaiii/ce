#include "ce/math.h"

#include "gtest/gtest.h"

#include <cmath>

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
    //ce::vec2<int32_t> v11{ 10, 10 };

    GTEST_EXPECT_TRUE(ce::inside(v01, v10));
    GTEST_EXPECT_TRUE(ce::inside(v00, v10));
    GTEST_EXPECT_TRUE(!ce::inside(v10, v01));
    GTEST_EXPECT_TRUE(!ce::inside(v10, v00));
}

GTEST_TEST(math, isqrt)
{
    volatile int32_t x = 0x01234567;
    auto xx = ce::sqrx(x);
    int32_t ix = ce::isqrt(xx);
    EXPECT_EQ(x, ix);
}

GTEST_TEST(math, cordic)
{
    EXPECT_EQ(ce::polar_ang(100, 0), ce::angle::_0);
    EXPECT_EQ(ce::polar_ang(100, 100), ce::angle::_45);
    EXPECT_EQ(ce::polar_ang(0, 100), ce::angle::_90);
    EXPECT_EQ(ce::polar_ang(-100, 100), ce::angle::_135);

    // these return negative rotations, reduced rotations are [-180, 180)
    EXPECT_EQ(ce::polar_ang(-100, 0), -ce::angle::_180);
    EXPECT_EQ(ce::polar_ang(-100, -100), -ce::angle::_135);
    EXPECT_EQ(ce::polar_ang(0, -100), -ce::angle::_90);
    EXPECT_EQ(ce::polar_ang(100, -100), -ce::angle::_45);

    EXPECT_EQ(ce::polar_mag(300, 400), 500);

    double pi = 3.141592653589793238462643383279502884197169399375105820974944;

    ASSERT_EQ(ce::detail::c_pi, pi);

    constexpr int32_t N = 1 << 10;

    for (int i = 0; i < N * 2; ++i)
    {
        constexpr int32_t scale = 1 << 23;

        auto c = ce::cos_2pi(scale, ce::angle::_180 / N * i);
        double d = std::cos(pi / N * i);
        int32_t f = int32_t(d * scale + (d < 0 ? -0.5 : +0.5));

        auto delta = c - f;
        if (delta < 0)
            delta = -delta;
        
        EXPECT_LT(delta, 1) << "i = " << i << ", cos_2pi = " << c / double(scale) << ", std::cos() = " << d;
    }
}


GTEST_TEST(math, degrees_to_angle)
{
    EXPECT_EQ(ce::angle::_0, ce::degrees_to_angle(0));
    EXPECT_EQ(ce::angle::_45, ce::degrees_to_angle(45));
    EXPECT_EQ(ce::angle::_90, ce::degrees_to_angle(90));
    EXPECT_EQ(ce::angle::_135, ce::degrees_to_angle(135));
    EXPECT_EQ(-ce::angle::_180, ce::degrees_to_angle(-180));
    EXPECT_EQ(-ce::angle::_135, ce::degrees_to_angle(-135));
    EXPECT_EQ(-ce::angle::_90, ce::degrees_to_angle(-90));
    EXPECT_EQ(-ce::angle::_45, ce::degrees_to_angle(-45));
}
