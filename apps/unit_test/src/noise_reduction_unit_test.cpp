#include "noise_reduction/noise_reduction_apriori.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(NoiseReduction, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<NoiseReductionAPriori>()); }

TEST(NoiseReduction, ML)
{
    auto c = NoiseReductionConfiguration::Coefficients();
    c.algorithmType = NoiseReductionConfiguration::Coefficients::ML;
    NoiseReduction noiseReduction(c);
}