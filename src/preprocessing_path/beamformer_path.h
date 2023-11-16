#pragma once
#include "framework/framework.h"
#include "algorithm_library/preprocessing_path.h"
#include "filterbank/filterbank_wola.h"
#include "beamformer/beamformer_mvdr.h"
#include "noise_estimation/noise_estimation_activity_detection.h"

class BeamformerPath : public AlgorithmImplementation<PreprocessingPathConfiguration, BeamformerPath>
{
public:
    BeamformerPath(const Coefficients& c = Coefficients()) :
        AlgorithmImplementation<PreprocessingPathConfiguration, BeamformerPath>{c}
    {
        filterbank.setStandardFilterbank(c.bufferSize, c.nChannels);
        filterbankInverse.setStandardFilterbank(c.bufferSize, 1); // only output 1 channel
        
        nBands = filterbank.getCoefficients().fftSize / 2 + 1; // get fftSize from filterbank
        BeamformerMVDR::Coefficients cBeamformer;
        cBeamformer.filterbankRate = c.sampleRate / c.bufferSize;
        cBeamformer.nBands = nBands;
        cBeamformer.nChannels = c.nChannels;
        beamformer.setCoefficients(cBeamformer);  // delay initialization of beamformer to after initializer list to get fftSize from filterbank
    }

    FilterbankAnalysisWOLA filterbank;
    FilterbankSynthesisWOLA filterbankInverse;
    BeamformerMVDR beamformer;
    DEFINE_MEMBER_ALGORITHMS(filterbank, filterbankInverse, beamformer)

    void processOn(Input input, Output output)
	{
        Eigen::ArrayXXcf xFreq(nBands, C.nChannels);
        Eigen::ArrayXcf xBeamformed(nBands);
		filterbank.process(input, xFreq);
        beamformer.process({xFreq, true}, xBeamformed); // TODO: replace true with VAD
        filterbankInverse.process(xBeamformed, output);
	}

    int getDelaySamples() const { return filterbank.getCoefficients().fftSize; } // this assumes filterbank is initialized as a standard filterbank

private:
    int nBands;
};