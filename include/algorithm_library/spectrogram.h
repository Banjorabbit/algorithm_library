#pragma once
#include "interface/interface.h"

// spectrogram. Default windows is Hann window, and there is a method for setting a user defined window.
// There is also a static method for getting the number of output frames given the input size.
//
// author: Kristian Timm Andersen

struct SpectrogramConfiguration
{
    using Input = I::Real;
    using Output = O::Real2D;

    struct Coefficients
    {
        int bufferSize = 128;
        int nBands = 257;
        enum SpectrogramAlgorithmType { HANN, SQRT_HANN, WOLA, NONLINEAR };
        SpectrogramAlgorithmType algorithmType = HANN; // choose algorithm to use for calculating spectrogram
        DEFINE_TUNABLE_ENUM(SpectrogramAlgorithmType, {{HANN, "Hann"}, {SQRT_HANN, "Sqrt Hann"}, {WOLA, "Wola"}, {NONLINEAR, "NonLinear"}})
        DEFINE_TUNABLE_COEFFICIENTS(bufferSize, nBands, algorithmType)
    };

    struct Parameters
    {
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    static inline int getNFrames(int nSamples, int bufferSize) { return nSamples / bufferSize; } // number of frames (columns) in output
    static int getValidFFTSize(int fftSize);                                                     // return valid FFT size larger or equal to fftSize

    static Eigen::ArrayXf initInput(const Coefficients &c) { return Eigen::ArrayXf::Random(10 * c.bufferSize); } // time samples. Number of samples is arbitrary

    static Eigen::ArrayXXf initOutput(Input input, const Coefficients &c) // power spectrogram
    {
        return Eigen::ArrayXXf::Zero(c.nBands, getNFrames(input.rows(), c.bufferSize));
    }

    static bool validInput(Input input, const Coefficients &c) { return (input.rows() > 0) && input.allFinite(); }

    static bool validOutput(Output output, const Coefficients &c) { return (output.rows() == c.nBands) && (output.cols() > 0) && output.allFinite(); }
};

class Spectrogram : public Algorithm<SpectrogramConfiguration>
{
  public:
    Spectrogram() = default;
    Spectrogram(const Coefficients &c);
};
