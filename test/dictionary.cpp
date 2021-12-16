#include "ce/dictionary.h"

#include "gtest/gtest.h"

struct index_t
{
    int value;
    int other_value;
};

using hash_to_index = ce::dictionary<256, index_t, uint32_t>;

GTEST_TEST(dictionary, test)
{
    hash_to_index d{ };

    constexpr auto mallori = ce::hash::fnv1a("Mallori");

    d.bind(mallori, 1, 2);
    d.bind(ce::hash::fnv1a("Jimmy"), 0, 9);
    d.bind(ce::hash::fnv1a("Sydney"), index_t{ 0, 9 });

    // entry 0 is not used so 4 instead of 3
    GTEST_EXPECT_TRUE(d.size == 4);

    GTEST_EXPECT_TRUE(d[mallori].value == 1);
    GTEST_EXPECT_TRUE(d[ce::hash::fnv1a("Sydney")].value == 0);
    GTEST_EXPECT_TRUE(&d[ce::hash::fnv1a("Sydney")] != &d[ce::hash::fnv1a("Jimmy")]);
    GTEST_EXPECT_TRUE(d[ce::hash::fnv1a("Sydney")].other_value == d[ce::hash::fnv1a("Jimmy")].other_value);
}

