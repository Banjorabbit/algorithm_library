#pragma once
#include "interface/interface.h"

// spectrogram. Default windows is Hann window, and there is a method for setting a user defined window.
// There is also a static method for getting the number of output frames given the input size.
//
// author: Kristian Timm Andersen

struct SpectrogramConfiguration : public Configuration<I::Real, O::Real2D>
{

	// choose algorithm to use for calculating spectrogram (set compile time)
	enum SpectrogramType { FILTERBANK, NONLINEAR};
	static constexpr SpectrogramType spectrogramType = FILTERBANK;

	struct Coefficients
	{
        int bufferSize = 128;
		int fftSize = 512;
		DEFINE_TUNABLE_COEFFICIENTS(bufferSize, fftSize)
	};

	template<typename Talgo>
	struct Test
	{
		Eigen::ArrayXf input;
		Eigen::ArrayXXf output;
		Test()
		{
            const int nFrames = 10;
			Coefficients c;
			input.resize(nFrames * c.bufferSize);
			input.setRandom();
			output.resize(c.fftSize / 2 + 1, nFrames);
		}

		inline void processAlgorithm(Talgo& algo) { algo.process(input, output); }
		bool isTestOutputFinite() const { return output.allFinite(); }
	};
};

class Spectrogram : public Algorithm<SpectrogramConfiguration>
{
public:
	Spectrogram() = default;
	Spectrogram(const Coefficients& c);

    void setWindow(I::Real window); // set FFT window
    static int getNFrames(int inputSize, int bufferSize); // get number of output frames given the size of the input signal and the bufferSize
	static int getValidFFTSize(int fftSize); // return valid FFT size larger or equal to fftSize
};
