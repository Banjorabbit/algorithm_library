#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "spectrogram/spectrogram_nonlinear.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(SpectrogramNonlinear, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<SpectrogramNonlinear>());
}

// description: test nFrames is a static function and that it returns correct number of frames.
TEST(SpectrogramNonlinear, getNFrames)
{
	SpectrogramNonlinear spec;
	auto c = spec.getCoefficients();
	const int bufferSize = c.bufferSize;
	const int nSamples = 1e4;
	const int nFrames = Spectrogram::getNFrames(nSamples, bufferSize); // test getNFrames() is a static method

	ArrayXf signal(nSamples);
	signal.setRandom();

	ArrayXXf output(c.fftSize / 2 + 1, nFrames + 1); // add one extra frame than needed
	output.setZero(); // important to set to zero, since we are checking last frame is zero in success criteria
	spec.process(signal, output);

	bool criteria = (!output.leftCols(nFrames).isZero()) && (output.rightCols(1).isZero()); // test criteria that all nFrames are non-zero and last frame is zero
	fmt::print("Criteria: {}\n", criteria);
	EXPECT_TRUE(criteria);
}

// description: process two spectrograms with the same input signal, set one of the spectrogram windows to a random window, and verify that the output spectrograms are different.
TEST(SpectrogramNonlinear, setWindow)
{
	SpectrogramNonlinear spec1;
	SpectrogramNonlinear spec2;

	auto c = spec2.getCoefficients();
	ArrayXf window(c.fftSize);
	window.setRandom();
	spec2.setWindow(window);

	const int nSamples = 1e5;
	const int nFrames = Spectrogram::getNFrames(nSamples, c.bufferSize);
	ArrayXf input(nSamples);
	input.setRandom();
	ArrayXXf output1(c.fftSize / 2 + 1, nFrames);
	ArrayXXf output2(c.fftSize / 2 + 1, nFrames);

	spec1.process(input, output1);
	spec2.process(input, output2);

	float diff = (output1 - output2).abs2().sum() / output1.abs2().sum();
	fmt::print("Diff: {}\n", diff);
	EXPECT_TRUE(diff > 1e-3); // error must be larger than threshold to show window was changed in spec2
}

// description: test that default window is equal to setWindows(hann window)
TEST(SpectrogramNonlinear, setWindowsDefault)
{
    SpectrogramNonlinear spec;
    ArrayXf window0 = spec.filterbank0.getWindow();
    ArrayXf window1 = spec.filterbank1.getWindow();
    ArrayXf window2 = spec.filterbank2.getWindow();

    spec.setWindow(hann(window0.size()));

    float error = (window0 - spec.filterbank0.getWindow()).abs2().sum() / window0.abs2().sum();
    fmt::print("Error0: {}\n", error);
    EXPECT_TRUE(error == 0.f);

    error = (window1 - spec.filterbank1.getWindow()).abs2().sum() / window1.abs2().sum();
    fmt::print("Error1: {}\n", error);
    EXPECT_TRUE(error == 0.f);

    error = (window2 - spec.filterbank2.getWindow()).abs2().sum() / window2.abs2().sum();
    fmt::print("Error2: {}\n", error);
    EXPECT_TRUE(error == 0.f);
}