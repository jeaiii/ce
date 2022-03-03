#include "ce/cdt.h"

#include "gtest/gtest.h"

// TODO jea - namespace cdt properly

GTEST_TEST(cdt, cdt)
{
    static ce::cdt<10, 4096> m{};

    m.reset();
    auto i = m.locate(0, 0, 0);
    GTEST_EXPECT_TRUE(i == 31);
}