#include "ce/sort.h"

#include "gtest/gtest.h"

GTEST_TEST(sort, intro_sort)
{
    int a[]{ 10, 2, 6, 2, 5, 7, 4, 9, 12, 10 };

    {
        // lower insertion_sort cutoff so we actually test quicksort
        ce::intro_sort<2>(a, a + CE_COUNTOF(a));
        int i = a[0];
        for (auto x : a)
        {
            GTEST_EXPECT_TRUE(i <= x);
            i = x;
        }
    }

    // sort sorted array
    {
        // lower insertion_sort cutoff so we actually test quicksort
        ce::intro_sort<2>(a, a + CE_COUNTOF(a));
        int i = a[0];
        for (auto x : a)
        {
            GTEST_EXPECT_TRUE(i <= x);
            i = x;
        }
    }

    {
        // actually tests insertion_sort
        int b[]{ 10, 2, 6, 2, 5, 7, 4, 9, 12, 10 };
        ce::intro_sort(b, b + CE_COUNTOF(a));
        int i = a[0];
        for (auto x : a)
        {
            GTEST_EXPECT_TRUE(i <= x);
            i = x;
        }
    }
}

GTEST_TEST(sort, insertion_sort)
{
    int b[]{ 10, 2, 6, 2, 5, 7, 4, 9, 12, 10 };
    ce::insertion_sort(b, b + CE_COUNTOF(b));
    int i = b[0];
    for (auto x : b)
    {
        GTEST_EXPECT_TRUE(i <= x);
        i = x;
    }
}
