#pragma once
#include "algorithm_library/fft.h"
#include "algorithm_library/filterbank.h"
#include "interface/interface.h"

// A set of analysis filter banks.
//
// It currently only supports:
// 1 channel input
// offline processing where the entire input is available
//
// The filterbanks are processed in parallel with FFT size doubling between each filterbank and output complex spectrograms are stored in a vector.
//
// author: Kristian Timm Andersen

struct FilterbankSetAnalysisConfiguration
{
    using Input = I::Real;
    using Output = O::VectorComplex2D;

    struct Coefficients
    {
        int bufferSize = 128; // buffer size in the first filterbank
        int nBands = 257;     // number of frequency bands in the first filterbank
        int nFilterbanks = 4; // each filterbank doubles the buffer size

        DEFINE_TUNABLE_COEFFICIENTS(bufferSize, nBands, nFilterbanks)
    };

    struct Parameters
    {
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    static Eigen::ArrayXf initInput(const Coefficients &c) { return Eigen::ArrayXf::Random(10000); } // arbitrary number of time samples.

    static std::vector<Eigen::ArrayXXcf> initOutput(Input input, const Coefficients &c)
    {
        std::vector<Eigen::ArrayXXcf> output(c.nFilterbanks);
        for (auto i = 0; i < c.nFilterbanks; i++)
        {
            int bufferSize = c.bufferSize * (1 << i);
            int nFrames = static_cast<int>(input.size()) / bufferSize;
            int fftSize = FFTConfiguration::convertNBandsToFFTSize(c.nBands) * (1 << i);
            int nBands = FFTConfiguration::convertFFTSizeToNBands(fftSize);
            output[i] = Eigen::ArrayXXcf::Zero(nBands, nFrames);
        }
        return output;
    }

    static bool validInput(Input input, const Coefficients &c) { return (input.rows() > 0) && (input.cols() == 1) && input.allFinite(); }

    static bool validOutput(Output output, const Coefficients &c)
    {
        if (static_cast<int>(output.size()) != c.nFilterbanks) { return false; }
        int nFrames = output[0].cols();
        for (auto i = 0; i < c.nFilterbanks; i++)
        {
            int fftSize = FFTConfiguration::convertNBandsToFFTSize(c.nBands) * (1 << i);
            int nBands = FFTConfiguration::convertFFTSizeToNBands(fftSize);
            if ((output[i].rows() != nBands) || (output[i].cols() != nFrames) || (!output[i].allFinite())) { return false; }
            nFrames /= 2;
        }
        return true;
    }
};

// Analysis filterbank
class FilterbankSetAnalysis : public Algorithm<FilterbankSetAnalysisConfiguration>
{
  public:
    FilterbankSetAnalysis() = default;
    FilterbankSetAnalysis(const Coefficients &c);

    float getDelaySamples() const;
};