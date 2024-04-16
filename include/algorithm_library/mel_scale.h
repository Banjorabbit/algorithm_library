#pragma once
#include "interface/interface.h"

// Mel scale
//
// author: Kristian Timm Andersen

struct MelScaleConfiguration
{
    using Input = I::Real2D;
    using Output = O::Real2D;

    struct Coefficients
    {
        int nMels = 40;           // output size
        int nBands = 513;         // input size
        float sampleRate = 16000; // Hz
        DEFINE_TUNABLE_COEFFICIENTS(nMels, nBands, sampleRate)
    };

    struct Parameters
    {
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    static Eigen::ArrayXXf initInput(const Coefficients &c) { return Eigen::ArrayXXf::Random(c.nBands, 2).abs2(); } // power spectrum. number of channels is arbitrary

    static Eigen::ArrayXXf initOutput(Input input, const Coefficients &c) { return Eigen::ArrayXXf::Zero(c.nMels, input.cols()); } // mel-scale power spectrum

    static bool validInput(Input input, const Coefficients &c) { return (input.cols() > 0) && (input.rows() == c.nBands) && (input >= 0).all(); }

    static bool validOutput(Output output, const Coefficients &c) { return (output.cols() > 0) && (output.rows() == c.nMels) && (output >= 0).all(); }
};

class MelScale : public Algorithm<MelScaleConfiguration>
{
  public:
    MelScale() = default;
    MelScale(const Coefficients &c);

    Eigen::ArrayXf getCornerFrequencies() const;
};
