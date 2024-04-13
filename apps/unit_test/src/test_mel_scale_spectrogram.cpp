#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "mel_scale/mel_scale_spectrogram.h"
#include <fmt/ranges.h>

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(MelScaleSpectrogram, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<MelScaleSpectrogram>());
}

// description: check that get corner frequencies work
TEST(MelScaleSpectrogram, getters)
{
	MelScaleSpectrogram algo;
    auto c = algo.getCoefficients();
	ArrayXf cornerFreqs = algo.getCornerFrequencies();
    fmt::print("Sample rate: {} Hz\n", c.sampleRate);
	fmt::print("Corner frequencies (Hz): {}\n", cornerFreqs);

 	fmt::print("Setting new sample rate...\n");
	
	c.sampleRate = 48000;
    algo.setCoefficients(c);
	fmt::print("Sample rate: {} Hz\n", c.sampleRate);
 	cornerFreqs = algo.getCornerFrequencies();
 	fmt::print("Corner frequencies (Hz): {}\n", cornerFreqs);
}

// process MelScaleSpectrogram with an input of ones, and invert the output. Check the inverse is equal to input.
TEST(MelScaleSpectrogram, processInverse)
{
    MelScaleSpectrogram algo;
    auto c = algo.getCoefficients();

    Eigen::ArrayXf input(c.nBands);
    input.setOnes();
    Eigen::ArrayXf output(c.nMels);
    Eigen::ArrayXf inverse(c.nBands);

    algo.process(input, output);
    algo.inverse(output, inverse);

    float error = (input - inverse).abs2().sum() / input.abs2().sum();
    fmt::print("Test error: {}\n", error);

    EXPECT_LT(error, 1e-10f);


}