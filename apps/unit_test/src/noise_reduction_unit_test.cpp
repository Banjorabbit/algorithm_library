#include "noise_reduction/noise_reduction_apriori.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(NoiseReduction, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<NoiseReductionAPriori>()); }
