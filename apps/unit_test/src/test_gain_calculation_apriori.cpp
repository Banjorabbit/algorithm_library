#include "framework/unit_test.h"
#include "gain_calculation/gain_calculation_apriori.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(GainCalculationApriori, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<GainCalculationApriori>()); }
