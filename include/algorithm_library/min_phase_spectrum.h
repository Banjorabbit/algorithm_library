#pragma once
#include "interface/interface.h"

// Calculate complex-valued minimum phase spectrum from a magnitude spectrum.
//
// author: Kristian Timm Andersen


struct MinPhaseSpectrumConfiguration
{
    using Input = I::Real2D;
    using Output = O::Complex2D;

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

    static auto validInput(Input input, const Coefficients& c) { return (input.rows() == c.nBands) && (input.cols() > 0); }
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXcf(c.nBands, input.cols()); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        int nChannels = 2;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXcf output;

        Test() : Test(Coefficients()) {}
        Test(const Coefficients& c) : algo(c)
        {
            input = Eigen::ArrayXXf::Random(c.nBands, nChannels).abs2();
            output = initOutput(input, c);
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
};

