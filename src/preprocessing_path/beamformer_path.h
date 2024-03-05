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
    BeamformerPath(const Coefficients& c = Coefficients()) :
        AlgorithmImplementation<PreprocessingPathConfiguration, BeamformerPath>{c},
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
    }

    FilterbankAnalysisWOLA filterbank;
    FilterbankSynthesisWOLA filterbankInverse;
    BeamformerMVDR beamformer;
    ActivityDetectionFusedNoiseEstimation activityDetector;
    DCRemoverFirstOrder dcRemover;
    DEFINE_MEMBER_ALGORITHMS(filterbank, filterbankInverse, beamformer, activityDetector, dcRemover)

    int getDelaySamples() const { return filterbank.getDelaySamples() + filterbankInverse.getDelaySamples(); } 

private:

    void processOn(Input input, Output output)
	{
        Eigen::ArrayXXf xTime(C.bufferSize, C.nChannels);
        Eigen::ArrayXXcf xFreq(nBands, C.nChannels);
        Eigen::ArrayXcf xBeamformed(nBands);
        bool activity;
        dcRemover.process(input, xTime);
		filterbank.process(xTime, xFreq);
        activityDetector.process(xFreq.abs2(), activity);
        beamformer.process({xFreq, activity}, xBeamformed); 
        filterbankInverse.process(xBeamformed, output);
	}
    
    int nBands;

    friend AlgorithmImplementation<PreprocessingPathConfiguration, BeamformerPath>;
};