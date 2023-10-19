#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "filter_min_max/filter_min_max_lemire.h"

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(FilterMinMaxLemire, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<FilterMinMaxLemire>());
}
