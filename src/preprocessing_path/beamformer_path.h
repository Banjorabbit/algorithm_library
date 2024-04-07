#pragma once
#include "framework/framework.h"
#include "algorithm_library/preprocessing_path.h"
#include "filterbank/filterbank_wola.h"
#include "beamformer/beamformer_mvdr.h"
#include "activity_detection/activity_detection_noise_estimation.h"
#include "dc_remover/dc_remover_first_order.h"

class BeamformerPath : public AlgorithmImplementation<PreprocessingPathConfiguration, BeamformerPath>
{
    int nBands; // It's important nBands is declared here before member algorithms since placement order determines the initialization order in constructor initializer list

public:
    BeamformerPath(const Coefficients& c = Coefficients()) : BaseAlgorithm{c},
        nBands{FFTConfiguration::getNBands(4 * c.bufferSize)}, // important to define this before member algorithms, since nBands is passed to constructors!
        filterbank(       {.nChannels = c.nChannels, .bufferSize = c.bufferSize, .nBands = nBands, .filterbankType = FilterbankAnalysisConfiguration::Coefficients::HANN}),
        filterbankInverse({.nChannels = 1,           .bufferSize = c.bufferSize, .nBands = nBands, .filterbankType = FilterbankSynthesisConfiguration::Coefficients::HANN}), // only 1 channel
        beamformer({.nChannels = c.nChannels, .filterbankRate = c.sampleRate / c.bufferSize, .nBands = nBands }),
        activityDetector({.filterbankRate = c.sampleRate / c.bufferSize, .nBands = nBands, .nChannels = c.nChannels}),
        dcRemover({.nChannels = c.nChannels, .sampleRate = c.sampleRate})
    {   
        xTime.resize(C.bufferSize, C.nChannels);
        xFreq.resize(nBands, C.nChannels);
        xFreq2.resize(nBands, C.nChannels);
        xBeamformed.resize(nBands);
    }

    FilterbankAnalysisWOLA filterbank;
    FilterbankSynthesisWOLA filterbankInverse;
    BeamformerMVDR beamformer;
    ActivityDetectionFusedNoiseEstimation activityDetector;
    DCRemoverFirstOrder dcRemover;
    DEFINE_MEMBER_ALGORITHMS(filterbank, filterbankInverse, beamformer, activityDetector, dcRemover)

    int getDelaySamples() const { return filterbank.getDelaySamples() + filterbankInverse.getDelaySamples(); } 

private:

    size_t getDynamicSizeVariables() const final 
    {
        size_t size = xTime.getDynamicMemorySize();
        size += xFreq.getDynamicMemorySize();
        size += xFreq2.getDynamicMemorySize();
        size += xBeamformed.getDynamicMemorySize();
        return size; 
    }

    void processOn(Input input, Output output)
    {
        dcRemover.process(input, xTime);
        filterbank.process(xTime, xFreq);
        xFreq2 = xFreq.abs2();
        activityDetector.process(xFreq2, activity);
        beamformer.process({xFreq, activity}, xBeamformed); 
        filterbankInverse.process(xBeamformed, output);
    }

    Eigen::ArrayXXf xTime;
    Eigen::ArrayXXcf xFreq;
    Eigen::ArrayXXf xFreq2;
    Eigen::ArrayXcf xBeamformed;
    bool activity;

    friend BaseAlgorithm;
};