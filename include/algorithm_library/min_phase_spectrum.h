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
    struct Example
    {
        Talgo algo;
        int nChannels, nBands;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXcf output;

        Example() : Example(Coefficients()) {}
        Example(const Coefficients& c) : algo(c)
        {
            nBands = c.nBands;
            nChannels = 2;
            input = Eigen::ArrayXXf::Random(nBands, nChannels).abs2();
            output = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, output); }
        bool isExampleOutputValid() const { return output.allFinite() && (output.rows() == nBands) && (output.cols() == nChannels); }
    };
};

class MinPhaseSpectrum : public Algorithm<MinPhaseSpectrumConfiguration>
{
public:
    MinPhaseSpectrum() = default;
    MinPhaseSpectrum(const Coefficients& c);
};

