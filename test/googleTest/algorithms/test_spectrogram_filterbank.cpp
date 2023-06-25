#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "spectrogram/spectrogram_filterbank.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(SpectrogramFilterbank, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<SpectrogramFilterbank>());
}

// description: test nFrames is a static function and that it returns correct number of frames.
TEST(SpectrogramFilterbank, getNFrames)
{
	Spectrogram spec;
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
TEST(SpectrogramFilterbank, setWindow)
{
	Spectrogram spec1;
	Spectrogram spec2;

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

	float error = (output1 - output2).abs2().sum() / output1.abs2().sum();
	fmt::print("Error: {}\n", error);
	EXPECT_TRUE(error > 1e-3); // error must be larger than threshold to show window was changed in spec2
}