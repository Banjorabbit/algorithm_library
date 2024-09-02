#pragma once
#include "algorithm_library/interface/interface.h"
#include "filterbank/filterbank_wola.h"
#include "framework/framework.h"

// Spectral selector between 3 inputs signals by choosing the one with minimum power.
//
// author: Kristian Timm Andersen

struct SpectralSelectorConfiguration
{
    using Input = I::Real2D;

    using Output = O::Real2D;

    struct Coefficients
    {
        int nChannels = 2; // number of columns in output is nChannels
        int nStreams = 3;  // number of columns in input is nChannels * nStreams where nChannels is interleaved
        int bufferSize = 256;
        DEFINE_TUNABLE_COEFFICIENTS(nChannels, nStreams, bufferSize)
    };

    struct Parameters
    {
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    static Eigen::ArrayXXf initInput(const Coefficients &c) { return Eigen::ArrayXXf::Random(c.bufferSize, c.nChannels * c.nStreams); }

    static Eigen::ArrayXXf initOutput(Input input, const Coefficients &c) { return Eigen::ArrayXXf::Zero(c.bufferSize, c.nChannels); }

    static bool validInput(Input input, const Coefficients &c) { return (input.rows() == c.bufferSize) && (input.cols() == c.nChannels * c.nStreams) && input.allFinite(); }

    static bool validOutput(Output output, const Coefficients &c) { return (output.rows() == c.bufferSize) && (output.cols() == c.nChannels) && output.allFinite(); }
};

class SpectralSelector : public AlgorithmImplementation<SpectralSelectorConfiguration, SpectralSelector>
{
  public:
    SpectralSelector(Coefficients c = Coefficients())
        : BaseAlgorithm{c}, filterbank({.nChannels = c.nChannels * c.nStreams,
                                        .bufferSize = c.bufferSize,
                                        .nBands = FFTConfiguration::convertFFTSizeToNBands(c.bufferSize * 4),
                                        .filterbankType = FilterbankAnalysisWOLA::Coefficients::HANN}),
          filterbankInverse({.nChannels = c.nChannels,
                             .bufferSize = c.bufferSize,
                             .nBands = FFTConfiguration::convertFFTSizeToNBands(c.bufferSize * 4),
                             .filterbankType = FilterbankAnalysisWOLA::Coefficients::HANN})
    {
        nBands = FFTConfiguration::convertFFTSizeToNBands(c.bufferSize * 4);
        filterbankOut.resize(nBands, c.nChannels * c.nStreams);
        powerMin.resize(nBands, c.nChannels);
        outputFreq.resize(nBands, c.nChannels);
        resetVariables();
    }

    FilterbankAnalysisWOLA filterbank;
    FilterbankSynthesisWOLA filterbankInverse;
    DEFINE_MEMBER_ALGORITHMS(filterbank, filterbankInverse)

  private:
    void inline processAlgorithm(Input input, Output output)
    {
        filterbank.process(input, filterbankOut);
        outputFreq = filterbankOut.leftCols(C.nChannels);
        powerMin = outputFreq.abs2();
        for (auto stream = 1; stream < C.nStreams; stream++)
        {
            for (auto channel = 0; channel < C.nChannels; channel++)
            {
                for (auto band = 0; band < nBands; band++)
                {
                    float power = std::norm(filterbankOut(band, channel + C.nChannels * stream));
                    if (power < powerMin(band, channel))
                    {
                        powerMin(band, channel) = power;
                        outputFreq(band, channel) = filterbankOut(band, channel + C.nChannels * stream);
                    }
                }
            }
        }
        filterbankInverse.process(outputFreq, output);
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = filterbankOut.getDynamicMemorySize();
        size += powerMin.getDynamicMemorySize();
        size += outputFreq.getDynamicMemorySize();
        return size;
    }

    void resetVariables() final
    {
        filterbankOut.setZero();
        powerMin.setZero();
        outputFreq.setZero();
    }

    int nBands;
    Eigen::ArrayXXcf filterbankOut;
    Eigen::ArrayXXcf outputFreq;
    Eigen::ArrayXXf powerMin;

    friend BaseAlgorithm;
};