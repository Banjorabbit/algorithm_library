#pragma once
#include "interface/interface.h"

// General implementation of a FFT-based filterbank that can be configured in multiple ways.
//
// author: Kristian Timm Andersen

// --------------------------------------------- Filterbank Analysis ----------------------------------------------------------------

struct FilterbankAnalysisConfiguration : public Configuration<I::Real2D, O::Complex2D>
{
	struct Coefficients
	{
		int nChannels = 2;
		int frameSize = 512;
		int fftSize = 512;
		int bufferSize = 128;
		float gain = 1;
		DEFINE_TUNABLE_COEFFICIENTS(nChannels, frameSize, fftSize, bufferSize, gain)
	};

	struct Parameters
	{
		enum WindowTypes { HANN_WINDOW, SQRT_HANN_WINDOW, RECTANGULAR, USER_DEFINED };
		WindowTypes windowType = HANN_WINDOW;
		DEFINE_TUNABLE_PARAMETERS(windowType)
	};

	static auto initInput(const Coefficients& c) { return Eigen::ArrayXXf(c.bufferSize, c.nChannels); }
	static auto initOutput(const Coefficients& c) { return Eigen::ArrayXXcf(c.fftSize/2 + 1, c.nChannels); }
	
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

// Analysis filterbank
class FilterbankAnalysis : public Algorithm<FilterbankAnalysisConfiguration>
{
public:
	FilterbankAnalysis() = default;
	FilterbankAnalysis(const Coefficients& c);
	FilterbankAnalysis(const Setup& s);

	void setWindow(I::Real window); // To have any effect, this method requires P.windowType == USER_DEFINED
	void setStandardFilterbank(int bufferSize = Coefficients().bufferSize, int nChannels = 1);
	void setLowDelayFilterbank(int bufferSize = Coefficients().bufferSize, int nChannels = 1);
	void setHighQualityFilterbank(int bufferSize = Coefficients().bufferSize, int nChannels = 1);
};

// --------------------------------------------- Filterbank Synthesis ----------------------------------------------------------------

struct FilterbankSynthesisConfiguration : public Configuration<I::Complex2D, O::Real2D, I::Real>
{
	struct Coefficients
	{
		int nChannels = 2;
		int frameSize = 512;
		int fftSize = 512;
		int bufferSize = 128;
		float gain = 1;
		DEFINE_TUNABLE_COEFFICIENTS(nChannels, frameSize, fftSize, bufferSize, gain)
	};

	struct Parameters
	{
		enum WindowTypes { HANN_WINDOW, SQRT_HANN_WINDOW, RECTANGULAR, USER_DEFINED };
		WindowTypes windowType = HANN_WINDOW;
		DEFINE_TUNABLE_PARAMETERS(windowType)
	};

	static auto initInput(const Coefficients& c) { return Eigen::ArrayXXcf(c.fftSize/2 + 1, c.nChannels); }
	static auto initOutput(const Coefficients& c) { return Eigen::ArrayXXf(c.bufferSize, c.nChannels); }

	template<typename Talgo>
	struct Test
	{
		Talgo algo;
		Eigen::ArrayXXcf input;
		Eigen::ArrayXXf output;

		Test(const Coefficients& c = {}) : algo(c), input(initInput(c)), output(initOutput(c))
		{
			input.setRandom();
		}

		inline void processAlgorithm() { algo.process(input, output); }
		bool isTestOutputFinite() const { return output.allFinite(); }

	};
};

class FilterbankSynthesis : public Algorithm<FilterbankSynthesisConfiguration>
{
public:
	FilterbankSynthesis() = default;
	FilterbankSynthesis(const Coefficients& c);
	FilterbankSynthesis(const Setup& s);

	void setWindow(I::Real window); // To have any effect, this method requires P.windowType == USER_DEFINED
	void setStandardFilterbank(int bufferSize = Coefficients().bufferSize, int nChannels = 1);
	void setLowDelayFilterbank(int bufferSize = Coefficients().bufferSize, int nChannels = 1);
	void setHighQualityFilterbank(int bufferSize = Coefficients().bufferSize, int nChannels = 1);
};