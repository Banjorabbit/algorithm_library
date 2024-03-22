#pragma once
#include "framework/framework.h"
#include "algorithm_library/preprocessing_path.h"
#include "filterbank/filterbank_wola.h"
#include "beamformer/beamformer_mvdr.h"
#include "activity_detection/activity_detection_noise_estimation.h"
#include "dc_remover/dc_remover_first_order.h"

class BeamformerPath : public AlgorithmImplementation<PreprocessingPathConfiguration, BeamformerPath>
{
public:
    BeamformerPath(const Coefficients& c = Coefficients()) : BaseAlgorithm{c},
        dcRemover({c.nChannels, c.sampleRate})
    {
        filterbank.setStandardFilterbank(c.bufferSize, c.nChannels);
        filterbankInverse.setStandardFilterbank(c.bufferSize, 1); // only output 1 channel
        
        nBands = filterbank.getCoefficients().fftSize / 2 + 1; // get fftSize from filterbank
        BeamformerMVDR::Coefficients cBeamformer;
        cBeamformer.filterbankRate = c.sampleRate / c.bufferSize;
        cBeamformer.nBands = nBands;
        cBeamformer.nChannels = c.nChannels;
        beamformer.setCoefficients(cBeamformer);  // delay initialization of beamformer to after initializer list to get fftSize from filterbank

        ActivityDetectionFusedNoiseEstimation::Coefficients cActivity;
        cActivity.filterbankRate = c.sampleRate / c.bufferSize;
        cActivity.nBands = nBands;
        cActivity.nChannels = c.nChannels;
        activityDetector.setCoefficients(cActivity);// delay initialization to after initializer list to get fftSize from filterbank

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
    
    int nBands;
    Eigen::ArrayXXf xTime;
    Eigen::ArrayXXcf xFreq;
    Eigen::ArrayXXf xFreq2;
    Eigen::ArrayXcf xBeamformed;
    bool activity;

    friend BaseAlgorithm;
};