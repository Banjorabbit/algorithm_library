#include "framework/framework.h"
#include "algorithm_library/spectrogram.h"
#include "filterbank/filterbank_wola.h"

class SpectrogramFilterbank : public AlgorithmImplementation<SpectrogramConfiguration, SpectrogramFilterbank>
{
public:
    SpectrogramFilterbank(Coefficients c = Coefficients()) : 
        AlgorithmImplementation<SpectrogramConfiguration, SpectrogramFilterbank>{c},
        filterbank(convertCoefficientsToFilterbankCoefficients(c))
    { 
        auto pFilterbank = filterbank.getParameters();
        pFilterbank.windowType = pFilterbank.HANN_WINDOW;
        filterbank.setParameters(pFilterbank);
        
        nBands = c.fftSize / 2 + 1;
    }

    FilterbankAnalysisWOLA filterbank;
    DEFINE_MEMBER_ALGORITHMS(filterbank)

    void inline processOn(Input input, Output output)
    {
        for (auto nFrame = 0; nFrame < getNFrames(static_cast<int>(input.size()), C.bufferSize); nFrame++)
        {
            Eigen::ArrayXf frame = input.segment(nFrame * C.bufferSize, C.bufferSize);
            Eigen::ArrayXcf filterbankOut(nBands);
            filterbank.process(frame, filterbankOut);
            output.col(nFrame) = filterbankOut.abs2();
        }
    }

    void setWindow(I::Real window) 
    {
        auto pFilterbank = filterbank.getParameters();
        pFilterbank.windowType = pFilterbank.USER_DEFINED;
        filterbank.setParameters(pFilterbank);
        filterbank.setWindow(window);
    }

    static inline int getNFrames(int nSamples, int bufferSize) { return nSamples / bufferSize; }

private:
    static FilterbankAnalysis::Coefficients convertCoefficientsToFilterbankCoefficients(Coefficients c) 
    {
        FilterbankAnalysis::Coefficients cFilterbank;
        cFilterbank.bufferSize = c.bufferSize;
        cFilterbank.fftSize = c.fftSize;
        cFilterbank.frameSize = c.fftSize;
        cFilterbank.gain = 1.f;
        cFilterbank.nChannels = 1;
        return cFilterbank;
    }

    int nBands;
};