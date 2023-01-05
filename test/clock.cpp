#include "ce/clock.h"

#include "gtest/gtest.h"

ce::clock<60> g_clock60hz;

GTEST_TEST(clock, 60hz)
{
    // no clock?
    if (ce::os::monotonic_frequency() == 0)
        return;

    auto a = ce::os::monotonic_timestamp();

    g_clock60hz.reset();

    uint64_t w = 0;
    while (g_clock60hz.current() < 30)
        ++w;

    auto b = ce::os::monotonic_timestamp();

    auto t = (b - a) / double(ce::os::monotonic_frequency());
    CE_LOG(clock, t, w);

    EXPECT_GE(t, .5 - .0125);
    EXPECT_LE(t, .5 + .0125);
}