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

    static auto validInput(Input input, const Coefficients& c) { return (input.rows() == c.fftSize) && (input.cols() > 0); }
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXcf(c.fftSize/2 +1, input.cols()); }

    // exception for constructing FFT with invalid FFT size
    class ExceptionFFT : public std::runtime_error {
    public:
        ExceptionFFT(int fftSize) : std::runtime_error("FFT size = " + std::to_string(fftSize) + " is not supported.") { }
    };

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        int nChannels = 2;
        int fftSize;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXcf output;

        Test() : Test(Coefficients()) {}
        Test(const Coefficients& c) : algo(c)
        {
            fftSize = c.fftSize;
            input = Eigen::ArrayXXf::Random(c.fftSize, nChannels);
            output = initOutput(input, c);
        }

        inline void processAlgorithm() { algo.process(input, output); }
        bool isTestOutputValid() const { return output.allFinite() && (output.rows() == fftSize/2 + 1) && (output.cols() == nChannels); }
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
