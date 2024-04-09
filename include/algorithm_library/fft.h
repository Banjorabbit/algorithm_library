#pragma once
#include "interface/interface.h"

// Real version of an FFT.
//
// This class has a public inverse FFT function.
// Supported FFT sizes are multiples of 32 and FFT class has public isFFTSizeValid() method.
// NOTE: This class will throw an exception if FFT size is not supported. 
//
// author: Kristian Timm Andersen

struct FFTConfiguration
{
    using Input = I::Real2D;
    using Output = O::Complex2D;

    struct Coefficients
    {
        int fftSize = 512;
        DEFINE_TUNABLE_COEFFICIENTS(fftSize)
    };

    struct Parameters { DEFINE_NO_TUNABLE_PARAMETERS };

    static int getFFTSize(int nBands) { return (nBands - 1) * 2; }
    static int getNBands(int fftSize) { return fftSize / 2 + 1; }

    static Eigen::ArrayXXf initInput(const Coefficients& c) { return Eigen::ArrayXXf::Random(c.fftSize, 2); } // time samples. Number of channels is arbitrary
    
    static Eigen::ArrayXXcf initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXcf::Zero(getNBands(c.fftSize), input.cols()); } // frequency bins. Number of channels is arbitrary

    static bool validInput(Input input, const Coefficients& c) { return (input.rows() == c.fftSize) && (input.cols() > 0) && input.allFinite(); }

    static bool validOutput(Output output, const Coefficients& c) { return (output.rows() == getNBands(c.fftSize)) && (output.cols() > 0) && output.allFinite(); }

    // exception for constructing FFT with invalid FFT size
    class ExceptionFFT : public std::runtime_error {
    public:
        ExceptionFFT(int fftSize) : std::runtime_error("FFT size = " + std::to_string(fftSize) + " is not supported.") { }
    };
};

class FFT : public Algorithm<FFTConfiguration>
{
public:
    FFT() = default;
    FFT(const Coefficients& c);

    void inverse(I::Complex2D xFreq, O::Real2D yTime);
    static bool isFFTSizeValid(const int fftSize);
};
