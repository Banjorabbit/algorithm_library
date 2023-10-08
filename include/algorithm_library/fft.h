#pragma once
#include "interface/interface.h"

// Real version of an FFT.
//
// This class has a public inverse FFT function.
// Supported FFT sizes are multiples of 32 and FFT class has public isFFTSizeValid() method.
// NOTE: This class will throw an exception if FFT size is not supported. 
//
// author: Kristian Timm Andersen

struct FFTConfiguration : public Configuration<I::Real2D, O::Complex2D>
{
	// exception for constructing FFT with invalid FFT size
	class ExceptionFFT : public std::runtime_error {
	public:
		ExceptionFFT(int fftSize) : std::runtime_error("FFT size = " + std::to_string(fftSize) + " is not supported.") { }
	};

	struct Coefficients
	{
		int fftSize = 512;
		int nChannels = 2;
		DEFINE_TUNABLE_COEFFICIENTS(fftSize, nChannels)
	};

	static auto initInput(const Coefficients& c) { return Eigen::ArrayXXf(c.fftSize, c.nChannels); }
	static auto initOutput(const Coefficients& c) { return Eigen::ArrayXXf(c.fftSize/2 +1, c.nChannels); }

	template<typename Talgo>
	struct Test
	{
		Talgo algo;
		Eigen::ArrayXXf input;
		Eigen::ArrayXXcf output;
		Test(const Coefficients& c = {}) : algo(c), input(initInput(c)), output(initOutput(c))
		{
			input.setRandom();
		}

		inline void processAlgorithm() { algo.process(input, output); }
		bool isTestOutputFinite() const { return output.allFinite(); }
	};
};

class FFT : public Algorithm<FFTConfiguration>
{
public:
	FFT() = default;
	FFT(const Coefficients& c);

	void inverse(I::Complex2D xFreq, O::Real2D yTime);
	static bool isFFTSizeValid(const int fftSize);
};
