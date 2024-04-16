#include "framework/unit_test.h"
#include "normal3d/normal3d_diff.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(Normal3d, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<Normal3dDiff>()); }

// description: check that the output changes at the time boundary when the algorithm is run 2 times
TEST(Normal3d, BoundaryTime)
{
    Normal3dDiff normalCalculation;
    auto c = normalCalculation.getCoefficients();

    const int nFrames = 10;
    ArrayXXf input(c.nValuesX, nFrames);
    input.setRandom();

    // run first time
    ArrayXXf output(c.nValuesX * 3, nFrames);
    normalCalculation.process(input, output);

    // run second time
    ArrayXXf output2(c.nValuesX * 3, nFrames);
    normalCalculation.process(input, output2);

    // error in first 2 columns should be non-zero
    float error = (output.leftCols(2) - output2.leftCols(2)).abs2().sum();
    fmt::print("Error in first 2 columns: {}\n", error);
    EXPECT_TRUE(error > 1e-6f); // threshold is some random small number

    // error in rest of the columns should be zero
    error = (output.rightCols(nFrames - 2) - output2.rightCols(nFrames - 2)).abs2().sum() / output.rightCols(nFrames - 2).abs2().sum();
    fmt::print("Error in rest of columns: {}\n", error);
    EXPECT_TRUE(error == 0.f);
}