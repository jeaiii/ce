#include "ce/sync.h"

#include "gtest/gtest.h"

ce::os::lock q;

GTEST_TEST(sync, sync)
{
    ce::os::construct_sync(q);
    ce::os::terminate_sync(q);

    ce::os::acquire_sync(q);
    ce::os::release_sync(q);
}