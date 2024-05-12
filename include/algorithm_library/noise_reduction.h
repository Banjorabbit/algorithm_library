#pragma once
#include "interface/interface.h"

struct NoiseReductionConfiguration
{
    using Input = I::Complex2D;
    using Output = O::Complex2D;

    struct Coefficients
    {
        int nBands = 128;
        int nChannels = 2;
        float filterbankRate = 16000.f;
        DEFINE_TUNABLE_COEFFICIENTS(nBands, nChannels, filterbankRate);
    };

    struct Parameters
    {
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    static Eigen::ArrayXXcf initInput(const Coefficients &c) { return Eigen::ArrayXXf::Random(c.nBands, c.nChannels).abs2(); } // complex input spectrum

    static Eigen::ArrayXXcf initOutput(Input input, const Coefficients &c)
    {
        return Eigen::ArrayXXf::Zero(c.nBands, c.nChannels); // complex output spectrum with noise reduction
    }

    static bool validInput(Input input, const Coefficients &c) { return (input.rows() == c.nBands) && (input.cols() == c.nChannels) && input.allFinite(); }

    static bool validOutput(Output output, const Coefficients &c) { return (output.rows() == c.nBands) && (output.cols() == c.nChannels) && output.allFinite(); }
};

class NoiseReduction : public Algorithm<NoiseReductionConfiguration>
{
  public:
    NoiseReduction() = default;
    NoiseReduction(const Coefficients &c);
};
