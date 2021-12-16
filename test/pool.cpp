#include "ce/pool.h"

#include "gtest/gtest.h"

GTEST_TEST(pool, append_tail)
{
    using pool = ce::pool<1024, int>;
    pool p;

    p.reset();

    using queue_t = pool::queue_t;

    queue_t q{};

    GTEST_EXPECT_TRUE(p.is_empty(q));

    p.append_tail(q, 0);
    p.append_tail(q, 1);
    p.append_tail(q, 2);

    GTEST_EXPECT_TRUE(!p.is_empty(q));

    GTEST_EXPECT_TRUE(p.head(q) == 0);
    p.remove_head(q);
    GTEST_EXPECT_TRUE(p.head(q) == 1);
    p.remove_head(q);
    GTEST_EXPECT_TRUE(p.head(q) == 2);
    p.remove_head(q);
}

GTEST_TEST(pool, insert_head)
{
    using pool = ce::pool<1024, int>;
    pool p;

    p.reset();

    using queue_t = pool::queue_t;

    queue_t q{};

    GTEST_EXPECT_TRUE(p.is_empty(q));

    p.insert_head(q, 2);
    p.insert_head(q, 1);
    p.insert_head(q, 0);

    GTEST_EXPECT_TRUE(!p.is_empty(q));

    GTEST_EXPECT_TRUE(p.head(q) == 0);
    p.remove_head(q);
    GTEST_EXPECT_TRUE(p.head(q) == 1);
    p.remove_head(q);
    GTEST_EXPECT_TRUE(p.head(q) == 2);
    p.remove_head(q);
}