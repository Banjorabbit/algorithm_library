#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "fft/fft_real.h"
using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(FFTReal, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<FFTReal>());
}


// Description: run FFT at default size, then invert it and check reconstruction is equal to input. Then change FFT size and run same test again
// pass/fail: Reconstruction errror is below a threshold
TEST(FFTReal, Reconstruction)
{
	FFTReal fft;

	// Run FFT and reconstruct at default FFT size
	auto c = fft.getCoefficients();
	int fftSize = c.fftSize;
	ArrayXf input(fftSize);
	input.setRandom();
	ArrayXf inputRef = input;
	ArrayXcf output(fftSize / 2 + 1);
	fft.process(input, output);
	input.setRandom(); // randomize input in case inverse doesn't write to input
	fft.inverse(output, input);

	float error = (input - inputRef).abs2().sum(); // calculate error
	fmt::print("Reconstruction error with FFT size = {0}: {1}\n", fftSize, error);

	EXPECT_LT(error, 1e-9f); // check error

	// Run same test again with new FFT size
	fftSize = 4096;
	c.fftSize = fftSize;
	fft.setCoefficients(c);
	input.resize(fftSize);
	inputRef.resize(fftSize);
	output.resize(fftSize);
	input.setRandom();
	inputRef = input;
	fft.process(input, output);
	input.setRandom(); // randomize input in case inverse doesn't write to input
	fft.inverse(output, input);

	error = (input - inputRef).abs2().sum();
	fmt::print("Reconstruction error with FFT size = {0}: {1}\n", fftSize, error);

	EXPECT_LT(error, 1e-9f);
}
// description: Choose an invalid FFT size and check FFT correctly detects it
// pass/fail: The FFT detects invalid size and throws exception
TEST(FFTReal, InvalidFFTSize)
{
	// set fftSize to 1023 (invalid size)
	int fftSize = 1023; // invalid FFT size;
	bool isValid = FFTReal::isFFTSizeValid(fftSize);
	EXPECT_FALSE(isValid);

	auto c = FFTReal::Coefficients();
	bool validFFTSize = true;
	try {
		c.fftSize = fftSize;
		FFTReal fft(c);
	}
	catch (FFTReal::Configuration::ExceptionFFT error) {
		fmt::print("Caught exception: {}\n", error.what());
		validFFTSize = false;
	}
	EXPECT_FALSE(validFFTSize);

	// set fftSize to 33 (invalid size)
	FFTReal fft;
	validFFTSize = true;
	try
	{
		c.fftSize = 33;
		fft.setCoefficients(c);
	}
	catch (FFTReal::Configuration::ExceptionFFT error) {
		fmt::print("Caught exception: {}\n", error.what());
		validFFTSize = false;
	}
	EXPECT_FALSE(validFFTSize);

	// set fftSize to 16 (invalid size)
	validFFTSize = true;
	try
	{
		c.fftSize = 16;
		fft.setCoefficients(c);
	}
	catch (FFTReal::Configuration::ExceptionFFT error) {
		fmt::print("Caught exception: {}\n", error.what());
		validFFTSize = false;
	}
	EXPECT_FALSE(validFFTSize);

	// set fftSize to 128 (valid size)
	validFFTSize = true;
	try
	{
		c.fftSize = 128;
		fft.setCoefficients(c);
	}
	catch (FFTReal::Configuration::ExceptionFFT error) {
		fmt::print("Caught exception: {}\n", error.what());
		validFFTSize = false;
	}
	EXPECT_TRUE(validFFTSize);

}

TEST(FFTReal, SIMDEnabled)
{
	bool test = pffft_simd_size() == 4;
	EXPECT_TRUE(test);
}