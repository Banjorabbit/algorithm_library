#pragma once
#include "interface/interface.h"

// spectrogram. Default windows is Hann window, and there is a method for setting a user defined window.
//
// author: Kristian Timm Andersen

struct SpectrogramConfiguration
{
    using Input = I::Real;
    using Output = O::Real2D;

    struct Coefficients
    {
        int bufferSize = 1024; // input buffer size
        int nBands = 1025;     // number of frequency bands in output
        enum SpectrogramAlgorithmType {
            HANN,            // Hann window with length of FFT size = 2 * (nBands - 1)
            WOLA,            // Sinc modulated Kaiser window with length of 2 * FFTSize = 4 * (nBands - 1)
            ADAPTIVE_HANN_8, // Hann window of adaptive length with max length of FFT size = 2 * (nBands - 1) and a min length of FFT size / 8
            ADAPTIVE_WOLA_8  // WOLA window of adaptive length with a max length of 2 * FFTSize = 4 * (nBands - 1) and a min length of 2 * FFT size / 8
        };
        SpectrogramAlgorithmType algorithmType = HANN; // choose algorithm to use for calculating spectrogram
        DEFINE_TUNABLE_ENUM(SpectrogramAlgorithmType, {{HANN, "Hann"}, {WOLA, "Wola"}, {ADAPTIVE_HANN_8, "Adaptive x8"}, {ADAPTIVE_WOLA_8, "Adaptive Wola x8"}})
        DEFINE_TUNABLE_COEFFICIENTS(bufferSize, nBands, algorithmType)
    };

    struct Parameters
    {
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    static int getValidFFTSize(int fftSize); // return valid FFT size larger or equal to fftSize

    static Eigen::ArrayXf initInput(const Coefficients &c) { return Eigen::ArrayXf::Random(c.bufferSize); } // time samples

    static Eigen::ArrayXXf initOutput(Input input, const Coefficients &c) // power spectrogram
    {
        if ((c.algorithmType == Coefficients::ADAPTIVE_HANN_8) || (c.algorithmType == Coefficients::ADAPTIVE_WOLA_8)) { return Eigen::ArrayXXf::Zero(c.nBands, 8); }
        return Eigen::ArrayXXf::Zero(c.nBands, 1);
    }

    static bool validInput(Input input, const Coefficients &c) { return (input.rows() == c.bufferSize) && input.allFinite(); }

    static bool validOutput(Output output, const Coefficients &c)
    {
        int nCols = 1;
        if ((c.algorithmType == Coefficients::ADAPTIVE_HANN_8) || (c.algorithmType == Coefficients::ADAPTIVE_WOLA_8)) { nCols = 8; }
        return (output.rows() == c.nBands) && output.allFinite() && (output >= 0).all() && (output.cols() == nCols);
    }
};

class Spectrogram : public Algorithm<SpectrogramConfiguration>
{
  public:
    Spectrogram() = default;
    Spectrogram(const Coefficients &c);
};
