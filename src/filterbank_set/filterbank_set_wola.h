#pragma once
#include "algorithm_library/filterbank_set.h"
#include "filterbank/filterbank_wola.h"
#include "framework/framework.h"

class FilterbankSetAnalysisWOLA : public AlgorithmImplementation<FilterbankSetAnalysisConfiguration, FilterbankSetAnalysisWOLA>
{
  public:
    FilterbankSetAnalysisWOLA(Coefficients c = Coefficients()) : BaseAlgorithm{c}, filterbanks(initializeFilterbanks(c))
    {
        bufferSizes.resize(c.nFilterbanks);
        for (auto i = 0; i < c.nFilterbanks; i++)
        {
            bufferSizes[i] = c.bufferSize * (1 << i);
        }
    }

    VectorAlgo<FilterbankAnalysisWOLA> filterbanks;
    DEFINE_MEMBER_ALGORITHMS(filterbanks)

  private:
    void processAlgorithm(Input input, Output output)
    {
        for (auto iFB = 0; iFB < C.nFilterbanks; iFB++)
        {
            const int nFrames = input.size() / bufferSizes[iFB];
            const int bufferSize = bufferSizes[iFB];
            for (auto iFrame = 0; iFrame < nFrames; iFrame++)
            {
                filterbanks[iFB].process(input.segment(iFrame * bufferSize, bufferSize), output[iFB].col(iFrame));
            }
        }
    }

    std::vector<FilterbankAnalysisWOLA::Coefficients> initializeFilterbanks(const Coefficients &c)
    {
        std::vector<FilterbankAnalysisWOLA::Coefficients> cFB(c.nFilterbanks);
        for (auto i = 0; i < c.nFilterbanks; i++)
        {
            cFB[i].bufferSize = c.bufferSize * (1 << i);
            cFB[i].filterbankType = cFB[i].HANN;
            cFB[i].nBands = FFTConfiguration::convertFFTSizeToNBands(4 * cFB[i].bufferSize);
            cFB[i].nChannels = 1;
        }
        return cFB;
    }

    size_t getDynamicSizeVariables() const final { return sizeof(int) * C.nFilterbanks; } // dynamic size of bufferSizes

    std::vector<int> bufferSizes;

    friend BaseAlgorithm;
};

struct FilterbankSetSynthesisConfiguration
{
    using Input = I::VectorComplex2D;
    using Output = O::Real;

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

    static std::vector<Eigen::ArrayXXcf> initInput(const Coefficients &c)
    {
        std::vector<Eigen::ArrayXXcf> input(c.nFilterbanks);
        for (auto i = 0; i < c.nFilterbanks; i++)
        {
            int bufferSize = c.bufferSize * (1 << i);
            int nFrames = input.size() / bufferSize;
            int fftSize = FFTConfiguration::convertNBandsToFFTSize(c.nBands) * (1 << i);
            int nBands = FFTConfiguration::convertFFTSizeToNBands(fftSize);
            input[i] = Eigen::ArrayXXcf::Random(nBands, nFrames);
        }
        return input;
    }

    static Eigen::ArrayXf initOutput(Input input, const Coefficients &c)
    {
        int nSamples = input[0].cols() * c.bufferSize;
        return Eigen::ArrayXf::Zero(nSamples);
    }

    static bool validInput(Input input, const Coefficients &c)
    {
        if (static_cast<int>(input.size()) != c.nFilterbanks) { return false; }
        int nFrames = input[0].cols();
        for (auto i = 0; i < c.nFilterbanks; i++)
        {
            int fftSize = FFTConfiguration::convertNBandsToFFTSize(c.nBands) * (1 << i);
            int nBands = FFTConfiguration::convertFFTSizeToNBands(fftSize);
            if ((input[i].rows() != nBands) || (input[i].cols() != nFrames) || (!input[i].allFinite())) { return false; }
            nFrames /= 2;
        }
        return true;
    }

    static bool validOutput(Output output, const Coefficients &c) { return (output.rows() > 0) && (output.cols() == 1) && output.allFinite(); }
};

class FilterbankSetSynthesisWOLA : public AlgorithmImplementation<FilterbankSetSynthesisConfiguration, FilterbankSetSynthesisWOLA>
{
  public:
    FilterbankSetSynthesisWOLA(Coefficients c = Coefficients()) : BaseAlgorithm{c}, inverseFilterbanks(initializeFilterbanks(c))
    {
        bufferOut.resize(c.nFilterbanks);
        for (auto i = 0; i < C.nFilterbanks; i++)
        {
            bufferOut[i] = Eigen::ArrayXf::Zero(c.bufferSize * (1 << i));
        }
        bufferSizes.resize(c.nFilterbanks);
        for (auto i = 0; i < c.nFilterbanks; i++)
        {
            bufferSizes[i] = c.bufferSize * (1 << i);
        }
    }

    VectorAlgo<FilterbankSynthesisWOLA> inverseFilterbanks;
    DEFINE_MEMBER_ALGORITHMS(inverseFilterbanks)

  private:
    void processAlgorithms(Input input, Output output) {}

    std::vector<FilterbankSynthesisWOLA::Coefficients> initializeFilterbanks(const Coefficients &c)
    {
        std::vector<FilterbankSynthesisWOLA::Coefficients> cFB(c.nFilterbanks);
        for (auto i = 0; i < c.nFilterbanks; i++)
        {
            cFB[i].bufferSize = c.bufferSize * (1 << i);
            cFB[i].filterbankType = cFB[i].HANN;
            cFB[i].nBands = FFTConfiguration::convertFFTSizeToNBands(4 * cFB[i].bufferSize);
            cFB[i].nChannels = 1;
        }
        return cFB;
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = sizeof(int) * C.nFilterbanks; // dynamic size of bufferSizes
        for (auto i = 0; i < static_cast<int>(bufferOut.size()); i++)
        {
            size += bufferOut[i].getDynamicMemorySize();
        }
        return size;
    }

    std::vector<int> bufferSizes;
    std::vector<Eigen::ArrayXf> bufferOut;

    friend BaseAlgorithm;
};