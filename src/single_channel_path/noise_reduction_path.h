#pragma once
#include "algorithm_library/single_channel_path.h"
#include "dc_remover/dc_remover_first_order.h"
#include "filterbank/filterbank_wola.h"
#include "framework/framework.h"
#include "gain_calculation/gain_calculation_apriori.h"
#include "noise_estimation/noise_estimation_activity_detection.h"

class NoiseReductionPath : public AlgorithmImplementation<SingleChannelPathConfiguration, NoiseReductionPath>
{
  public:
    NoiseReductionPath(const Coefficients &c = Coefficients())
        : BaseAlgorithm{c}, //
          filterbank({.nChannels = 1,
                      .bufferSize = c.bufferSize,
                      .nBands = FFTConfiguration::convertFFTSizeToNBands(4 * c.bufferSize),
                      .filterbankType = FilterbankConfiguration::Coefficients::HANN}),
          filterbankInverse({.nChannels = 1,
                             .bufferSize = c.bufferSize,
                             .nBands = FFTConfiguration::convertFFTSizeToNBands(4 * c.bufferSize),
                             .filterbankType = FilterbankConfiguration::Coefficients::HANN}),
          noiseEstimation({.filterbankRate = c.sampleRate / c.bufferSize, .nBands = FFTConfiguration::convertFFTSizeToNBands(4 * c.bufferSize), .nChannels = 1}),
          gainCalculation({.nChannels = 1, .filterbankRate = c.sampleRate / c.bufferSize, .nBands = FFTConfiguration::convertFFTSizeToNBands(4 * c.bufferSize)}),
          dcRemover({.nChannels = 1, .sampleRate = c.sampleRate})
    {
        int nBands = FFTConfiguration::convertFFTSizeToNBands(4 * c.bufferSize);
        xTime.resize(c.bufferSize);
        xFreq.resize(nBands);
        xFreq2.resize(nBands);
        noisePow.resize(nBands);
        gain.resize(nBands);
    }

    FilterbankAnalysisWOLA filterbank;
    FilterbankSynthesisWOLA filterbankInverse;
    NoiseEstimationActivityDetection noiseEstimation;
    GainCalculationApriori gainCalculation;
    DCRemoverFirstOrder dcRemover;
    DEFINE_MEMBER_ALGORITHMS(filterbank, filterbankInverse, noiseEstimation, gainCalculation, dcRemover)

    int getDelaySamples() const { return filterbank.getDelaySamples() + filterbankInverse.getDelaySamples(); }

  private:
    void processAlgorithm(Input input, Output output)
    {
        dcRemover.process(input, xTime);
        filterbank.process(xTime, xFreq);

        xFreq2 = xFreq.abs2();
        noiseEstimation.process(xFreq2, noisePow);

        xFreq2 /= noisePow.max(1e-16f);
        gainCalculation.process(xFreq2, gain);

        xFreq *= gain;
        filterbankInverse.process(xFreq, output);
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = xTime.getDynamicMemorySize();
        size += xFreq.getDynamicMemorySize();
        size += xFreq2.getDynamicMemorySize();
        size += noisePow.getDynamicMemorySize();
        size += gain.getDynamicMemorySize();
        return size;
    }

    Eigen::ArrayXf xTime;
    Eigen::ArrayXcf xFreq;
    Eigen::ArrayXf xFreq2;
    Eigen::ArrayXf noisePow;
    Eigen::ArrayXf gain;

    friend BaseAlgorithm;
};