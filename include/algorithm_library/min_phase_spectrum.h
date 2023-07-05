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
        DEFINE_TUNABLE_COEFFICIENTS(nBands)
    };

    struct Parameters
    {
        float minMagnitude = 1e-5f;
        DEFINE_TUNABLE_PARAMETERS(minMagnitude)
    };

	template<typename Talgo>
	struct Test
	{
		Eigen::ArrayXXf input;
        Eigen::ArrayXXcf output;

		Test()
		{
            int nChannels = 2;
            auto c = Coefficients();
            input = Eigen::ArrayXXf::Random(c.nBands, nChannels).abs2();
            output.resize(c.nBands, nChannels);
		}

		void processAlgorithm(Talgo& algo) { algo.process(input, output); }
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

