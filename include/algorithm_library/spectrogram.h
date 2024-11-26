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
        int bufferSize = 128; // input buffer size
        int nBands = 129;     // number of frequency bands in output
        enum SpectrogramAlgorithmType {
            HANN,          // HANN = Hann window with length of FFT size = 2 * (nBands - 1)
            WOLA,          // WOLA = sinc modulated Kaiser window with length of 2 * FFTSize = 4 * (nBands - 1)
            ADAPTIVE_HANN, // ADAPTIVE = Hann window of adaptive length with min length of 2 * bufferSize and max length of FFT size = 2 * (nBands - 1)
            ADAPTIVE_WOLA // ADAPTIVE_WOLA = sinc modulated Kaiser window of adaptive length with min length of 2 * bufferSize and max length of 2 * FFTSize = 4 * (nBands - 1)
        };
        SpectrogramAlgorithmType algorithmType = HANN; // choose algorithm to use for calculating spectrogram
        DEFINE_TUNABLE_ENUM(SpectrogramAlgorithmType, {{HANN, "Hann"}, {WOLA, "Wola"}, {ADAPTIVE_HANN, "Adaptive"}, {ADAPTIVE_WOLA, "Adaptive Wola"}})
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
        return Eigen::ArrayXXf::Zero(c.nBands, getNFrames(static_cast<int>(input.rows()), c.bufferSize));
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
