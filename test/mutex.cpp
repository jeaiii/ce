#include "ce/mutex.h"

#include "gtest/gtest.h"


GTEST_TEST(mutex, thread_mutex)
{
    static ce::thread_mutex q;

    ce::construct_mutex(q);

    ce::acquire_mutex(q);
    ce::release_mutex(q);

    GTEST_EXPECT_TRUE(ce::try_acquire_mutex(q));
    ce::release_mutex(q);
    GTEST_EXPECT_TRUE(ce::try_acquire_mutex(q));
    ce::release_mutex(q);

    ce::destroy_mutex(q);
}

GTEST_TEST(mutex, thread_shared_mutex)
{
    static ce::thread_shared_mutex q;

    ce::construct_mutex(q);

    ce::acquire_mutex(q);
    ce::release_mutex(q);

    GTEST_EXPECT_TRUE(ce::try_acquire_mutex(q));
    ce::release_mutex(q);
    GTEST_EXPECT_TRUE(ce::try_acquire_mutex(q));
    ce::release_mutex(q);

    GTEST_EXPECT_TRUE(ce::try_acquire_mutex_shared(q));
    ce::release_mutex_shared(q);
    GTEST_EXPECT_TRUE(ce::try_acquire_mutex_shared(q));
    ce::release_mutex_shared(q);

    ce::destroy_mutex(q);
}