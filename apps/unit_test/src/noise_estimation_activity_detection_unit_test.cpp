#include "noise_estimation/noise_estimation_activity_detection.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(NoiseEstimationActivityDetection, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<NoiseEstimationActivityDetection>()); }

TEST(NoiseEstimationActivityDetection, NoiseStationaryConvergence)
{
    srand(1); // reset random generator to get consistent results
    auto c = NoiseEstimationOutputActivityDetection::Coefficients();
    c.nChannels = 1;
    NoiseEstimationOutputActivityDetection noiseEstimation(c);

    ArrayXf powerInput = (ArrayXf::Random(c.nBands) + 1.f) * .5f; // array of random numbers between 0 and 1
    ArrayXf activity = ArrayXf::Zero(c.nBands);
    ArrayXf powerNoise(c.nBands);
    powerNoise.setZero();

    // run once and check that Activity == 1
    noiseEstimation.process(powerInput, {powerNoise, activity});
    float ADError = 1.f / c.nBands * ((activity - 1.f).abs()).sum();
    fmt::print("Activity detection mean error is: {}\n", ADError);
    EXPECT_LT(ADError, 1e-10);

    // run algorithm for 3 seconds
    int nEnd = int(3.f * c.filterbankRate); // number of samples corresponding to 3 seconds
    for (auto n = 0; n < nEnd; n++)
    {
        noiseEstimation.process(powerInput, {powerNoise, activity});
    }

    // calculate error
    float Error = 1.f / c.nBands * ((powerNoise - powerInput).abs2()).sum();
    fmt::print("Noise mean-square error is: {}\n", Error);
    EXPECT_LT(Error, 1e-10);

    // check Activity < .1
    ADError = 1.f / c.nBands * activity.sum();
    fmt::print("Activity mean error is: {}\n", ADError);
    EXPECT_LT(ADError, .1f);
}