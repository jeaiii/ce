#define CE_USER_CHECKED

#include "ce/ce.h"

#include "ce/atomic.h"
#include "ce/cdt.h"
#include "ce/dictionary.h"
#include "ce/io.h"
#include "ce/lziii.h"
#include "ce/math.h"
#include "ce/mutex.h"
#include "ce/nameof.h"
#include "ce/pool.h"
#include "ce/sort.h"
#include "ce/zorder.h"

#include "gtest/gtest.h"

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

enum class colors
{
    red,
    green,
    blue
};

void test_assert(int a)
{
    CE_ASSERT();
    CE_ASSERT(a < 3);
    CE_ASSERT(a < 3, a);
    CE_ASSERT(a < 3, a, a + a);
    CE_ASSERT(a < 3, "a is too big", a, a + a);
}

GTEST_TEST(ce, main)
{
    volatile int a = 4;


    CE_ASSERT(a > 1);
    CE_ASSERT(a > 1, a, "test");

    if (CE_VERIFY(a > 1))
    {
        CE_LOG_MSG(info, "GOOD");
    }
    else
    {
        CE_LOG_MSG(info, "BAD");
    }

    if (CE_REJECT(a > 4))
    {
        CE_LOG_MSG(info, "BAD");
    }
    else
    {
        CE_LOG_MSG(info, "GOOD");
    }

#if 0
    test_assert(a);

    if (CE_VERIFY(a < 2, a))
    {
        CE_LOG_MSG(info, "GOOD");
    }
    else
    {
        CE_LOG_MSG(info, "BAD");
    }

    if (CE_REJECT(a > 2, a))
    {
        CE_LOG_MSG(info, "BAD");
    }
    else
    {
        CE_LOG_MSG(info, "GOOD");
    }
#endif

    CE_LOG(0);
    CE_LOG(info, "Hello World", a, a * a, a / a, a + a, a - a, a + 1, a + 2, a + 3, a + 4, a + 5);
    CE_LOG(snowball, "Hello Snowball");

    CE_LOG(to_text, char('A'), bool(true));
    CE_LOG(to_text, int8_t(-1), int16_t(-1), int32_t(-1), int64_t(-1));
    CE_LOG(to_text, uint8_t(-1), uint16_t(-1), uint32_t(-1), uint64_t(-1));
    CE_LOG(to_text, (signed char)-1, (short)-1, (int)-1, (long)-1, (long long)-1);
    CE_LOG(to_text, (unsigned char)-1, (unsigned short)-1, (unsigned int)-1, (unsigned long)-1, (unsigned long long)-1);
    CE_LOG(to_text, float(10.5), double(100.5), float(0.123496875f));
    CE_LOG(to_text, colors::red, colors::green, colors::blue);

    int* ip = nullptr;// &a;

    ce::vec2<int> v2{ 0, 1 };
    ce::vec3<int> v3{ 0, 1, 2 };

    CE_LOG(text, v2);

    struct mine { };

    CE_LOG(to_text, nullptr, ip, v2, v3, mine{ });

    CE_LOG_MSG(messages, "Hello", ", ", "Earth ", a);


    CE_LOG_EX(7, "1", 1, 2);

    CE_LOG_MSG(info, "hello = ", 42);

    //CE_LOG(info, (4, "hello(\", j" ",", 5, "world"), (35));
    CE_LOG(info, (ce::vec2<int32_t, void>{ 1, 3 })        ,      ce::vec2<int32_t, void>{ 2, 5 });

    char long_string[1152 + 1];
    for (auto& c : long_string) c = 'Q';
    long_string[1152] = '\0';

    CE_LOG_MSG(info, (char const*)long_string);
    CE_LOG_MSG(info, '[', (char const*)long_string, ']');
}

GTEST_TEST(ce, bulk_simple)
{
    ce::bulk<4, int> b;

    GTEST_EXPECT_TRUE(b.size == 0);

    GTEST_EXPECT_TRUE(b.append(1));
    GTEST_EXPECT_TRUE(b.append(2));
    GTEST_EXPECT_TRUE(b.size == 2);

    GTEST_EXPECT_FALSE(b.append_n(10));
    GTEST_EXPECT_TRUE(b.size == 2);

    GTEST_EXPECT_TRUE(b.append(3));
    GTEST_EXPECT_TRUE(b.size == 3);

    GTEST_EXPECT_TRUE(b.append(4));
    GTEST_EXPECT_TRUE(b.size == 4);

    GTEST_EXPECT_FALSE(b.append(5));
    GTEST_EXPECT_TRUE(b.size == 4);

    GTEST_EXPECT_TRUE(b.remove_at(0));
    GTEST_EXPECT_TRUE(b.size == 3);
    EXPECT_EQ(b[0], 4);
    EXPECT_EQ(b[1], 2);
    EXPECT_EQ(b[2], 3);

    GTEST_EXPECT_TRUE(b.remove_at<true>(0));
    GTEST_EXPECT_TRUE(b.size == 2);
    EXPECT_EQ(b[0], 2);
    EXPECT_EQ(b[1], 3);
}

namespace test
{
    struct stats_t
    {
        int destroy;
        int construct;

        int copy_construct;
        int move_construct;
        int copy_assign;
        int move_assign;

        void clear()
        {
            destroy = 0;
            construct = 0;

            copy_construct = 0;
            move_construct = 0;
            copy_assign = 0;
            move_assign = 0;
        }

        bool expect(int d, int c, int cc, int mc, int ca, int ma)
        {
            EXPECT_EQ(destroy, d);
            EXPECT_EQ(construct, c);

            EXPECT_EQ(copy_construct, cc);
            EXPECT_EQ(move_construct, mc);
            EXPECT_EQ(copy_assign, ca);
            EXPECT_EQ(move_assign, ma);

            return destroy == d
                && construct == c
                && copy_construct == cc
                && move_construct == mc
                && copy_assign == ca
                && move_assign == ma;
        }
    };

    struct big
    {
        static stats_t stats;

        ~big() { ++stats.destroy; }
        big() { ++stats.construct; }
        big(big const&) { ++stats.copy_construct; }
        big(big&&) { ++stats.move_construct; }
        big& operator=(big const&) { ++stats.copy_assign; return *this; }
        big& operator=(big&&) noexcept { ++stats.move_assign; return *this; }
    };

    stats_t big::stats{ };
}

GTEST_TEST(ce, bulk_complex)
{
    test::big::stats.clear();

    {
        ce::bulk<3, test::big> b;

        GTEST_EXPECT_TRUE(b.size == 0);
        GTEST_EXPECT_TRUE(test::big::stats.expect(0, 0, 0, 0, 0, 0));

        GTEST_EXPECT_TRUE(b.append());
        GTEST_EXPECT_TRUE(test::big::stats.expect(0, 1, 0, 0, 0, 0));

        GTEST_EXPECT_TRUE(b.append());
        GTEST_EXPECT_TRUE(b.size == 2);
        GTEST_EXPECT_TRUE(test::big::stats.expect(0, 2, 0, 0, 0, 0));

        GTEST_EXPECT_FALSE(b.append_n(10));
        GTEST_EXPECT_TRUE(b.size == 2);
        GTEST_EXPECT_TRUE(test::big::stats.expect(0, 2, 0, 0, 0, 0));

        GTEST_EXPECT_TRUE(b.append());
        GTEST_EXPECT_TRUE(b.size == 3);
        GTEST_EXPECT_TRUE(test::big::stats.expect(0, 3, 0, 0, 0, 0));

        GTEST_EXPECT_FALSE(b.append());
        GTEST_EXPECT_TRUE(b.size == 3);
        GTEST_EXPECT_TRUE(test::big::stats.expect(0, 3, 0, 0, 0, 0));

        GTEST_EXPECT_TRUE(b.remove_at(0));
        GTEST_EXPECT_TRUE(b.size == 2);
        GTEST_EXPECT_TRUE(test::big::stats.expect(1, 3, 0, 0, 0, 1));

        GTEST_EXPECT_TRUE(b.remove_at(1));
        GTEST_EXPECT_TRUE(b.size == 1);
        GTEST_EXPECT_TRUE(test::big::stats.expect(2, 3, 0, 0, 0, 1));

        b.clear();
        GTEST_EXPECT_TRUE(b.size == 0);
        GTEST_EXPECT_TRUE(test::big::stats.expect(3, 3, 0, 0, 0, 1));

        GTEST_EXPECT_TRUE(b.append_n(3));
        GTEST_EXPECT_TRUE(b.size == 3);
        GTEST_EXPECT_TRUE(test::big::stats.expect(3, 6, 0, 0, 0, 1));

        GTEST_EXPECT_FALSE(b.append_n(3));
        GTEST_EXPECT_TRUE(b.size == 3);

        GTEST_EXPECT_TRUE(b.resize(2));
        GTEST_EXPECT_TRUE(b.size == 2);

        GTEST_EXPECT_FALSE(b.resize(6));
        GTEST_EXPECT_TRUE(b.size == 2);

        GTEST_EXPECT_TRUE(b.resize(0));
        GTEST_EXPECT_TRUE(test::big::stats.expect(6, 6, 0, 0, 0, 1));

        test::big i{ };
        b[0] = i;
        b[0] = b[1];
        b[0] = { };

        b[0] = test::big{ b[1] };

        b.append(i);
        b.append(test::big{ });
        b.append_n(1);
        b.remove_at<true>(0);
        b.append_n(1);
        b.remove_at(0);
    }
    GTEST_EXPECT_TRUE(test::big::stats.expect(14, 11, 2, 1, 2, 6));
}


template<class T> auto test_random(T& g)
{
    constexpr uint8_t data[] = "hello world";

    seed(g, CE_COUNTOF(data), data);

    int counts[16]{ };
    for (size_t i = 0; i < 1024 * 16; ++i)
        counts[next(g) % 16] += 1;

    int lo = counts[0];
    int hi = counts[0];
    int sos = 0;
    for (auto c : counts)
    {
        if (c < lo) lo = c;
        if (c > hi) hi = c;

        sos += (c - 1024) * (c - 1024);
    }

    CE_LOG(random, ce::nameof<T>(), lo, hi, sos);
    return sos < 256 * 256 * 3 / 8;
}

GTEST_TEST(ce, random_xoroshiro64ss)
{
    ce::random::xoroshiro64ss g;
    auto sos = test_random(g);
    EXPECT_TRUE(sos);
}

GTEST_TEST(ce, random_xoroshiro128pp)
{
    ce::random::xoroshiro128pp g;
    auto sos = test_random(g);
    EXPECT_TRUE(sos);
}

GTEST_TEST(ce, random_pcg32_64)
{
    ce::random::pcg32_64_t g;
    auto sos = test_random(g);
    EXPECT_TRUE(sos);
}

void get_timestamp_and_monotonic(uint64_t& timestamp, uint64_t& monotonic)
{
    auto t0 = CE_TIMESTAMP();
    auto m0 = ce::os::monotonic_timestamp();
    auto t1 = CE_TIMESTAMP();
    auto m1 = ce::os::monotonic_timestamp();
    auto t2 = CE_TIMESTAMP();

    if (t2 - t1 < t1 - t0)
    {
        t0 = t1;
        m0 = m1;
    }
    timestamp = t0;
    monotonic = m0;
}


GTEST_TEST(ce, monotonic_timestamp)
{
    auto f = ce::os::monotonic_frequency();

    if (f == 0)
        return;
    
    auto t0 = CE_TIMESTAMP();
    auto m0 = ce::os::monotonic_timestamp();
    auto t1 = CE_TIMESTAMP();
    auto m1 = ce::os::monotonic_timestamp();
    auto t2 = CE_TIMESTAMP();

    CE_LOG(info, t0, t1, t2);

    CE_LOG(info, f);
    CE_LOG(info, m0, m1);

    EXPECT_LT(t0, t1);
    EXPECT_LT(t1, t2);
    EXPECT_LE(m0, m1);

    auto dt0 = t1 - t0;
    auto dt1 = t2 - t1;

    CE_LOG(info, dt0, dt1);

    if (dt1 < dt0)
    {
        t0 = t1;
        m0 = m1;
    }

    uint64_t t;
    uint64_t m;
    do
    {
        get_timestamp_and_monotonic(t, m);
        m -= m0;

        if (m == 0)
            continue;

        t -= t0;

        auto qf = f / double(m);
        auto cf = qf * t;

        CE_LOG(info, cf);
        ce::os::sleep_ns(100000000);
    } while (m * 2 < f);
}