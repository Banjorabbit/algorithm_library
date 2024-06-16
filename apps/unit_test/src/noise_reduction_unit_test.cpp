#include "noise_reduction/noise_reduction_apriori.h"
#include "noise_reduction/noise_reduction_ml.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(NoiseReduction, InterfaceApriori) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<NoiseReductionAPriori>()); }

TEST(NoiseReduction, InterfaceML) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<NoiseReductionML>()); }

// test the ML noise reduction can be run
TEST(NoiseReduction, MLRun)
{
    NoiseReductionML noiseReduction;

    auto input = noiseReduction.initInput();
    auto output = noiseReduction.initOutput(input);

    noiseReduction.process(input, output);
}