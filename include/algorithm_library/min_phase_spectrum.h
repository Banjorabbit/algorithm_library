#pragma once
#include "interface/interface.h"

// Calculate complex-valued minimum phase spectrum from a magnitude spectrum.
//
// author: Kristian Timm Andersen


struct MinPhaseSpectrumConfiguration : public Configuration<I::Real2D, O::Complex2D>
{
    struct Coefficients
    {
        int nBands = 257;
        int nChannels = 2;
        DEFINE_TUNABLE_COEFFICIENTS(nBands, nChannels)
    };

    struct Parameters
    {
        float minMagnitude = 1e-5f;
        DEFINE_TUNABLE_PARAMETERS(minMagnitude)
    };

	template<typename Talgo>
	struct Test
	{
        Talgo algo;
		Eigen::ArrayXXf input;
        Eigen::ArrayXXcf output;

		Test(const Coefficients& c = {}) : algo(c)
		{
            input = Eigen::ArrayXXf::Random(c.nBands, c.nChannels).abs2();
            output.resize(c.nBands, c.nChannels);
		}

		void processAlgorithm() { algo.process(input, output); }
		bool isTestOutputFinite() const { return output.allFinite(); }
	};
};

class MinPhaseSpectrum : public Algorithm<MinPhaseSpectrumConfiguration>
{
public:
	MinPhaseSpectrum() = default;
    MinPhaseSpectrum(const Coefficients& c);
    MinPhaseSpectrum(const Setup& s);
};

