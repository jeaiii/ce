#include "ce/nameof.h"

#include "gtest/gtest.h"


enum class city
{
    _unknown,
    newport,
    irvine,
    costa_mesa,
    langua_beach
};

struct university
{
    enum test
    {
        _unknown,
        ucla,
        uci,
        berkeley,
    };
};

enum
{
    red,
    green,
    blue
};

GTEST_TEST(nameof, nameof)
{
    EXPECT_STREQ("irvine", ce::nameof(city::irvine));
    EXPECT_STREQ("???", ce::nameof(city(45)));
    EXPECT_STREQ("ucla", ce::nameof(university::ucla));
    EXPECT_STREQ("green", ce::nameof(green));
    EXPECT_EQ(blue, ce::as_enum<decltype(red)>("blue"));
    EXPECT_EQ(green, ce::as_enum("green", red));
    EXPECT_EQ(red, ce::as_enum("grey", red));
}