#include "framework/framework.h"
#include "algorithm_library/spectrogram.h"
#include "filterbank/filterbank_wola.h"

class SpectrogramFilterbank : public IAlgorithm<SpectrogramConfiguration, SpectrogramFilterbank>
{
public:
    SpectrogramFilterbank(Coefficients c = Coefficients()) : 
        IAlgorithm<SpectrogramConfiguration, SpectrogramFilterbank>{c},
        filterbank(convertCoefficientsToFilterbankSetup(c))
    { 
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
    static FilterbankAnalysis::Setup convertCoefficientsToFilterbankSetup(Coefficients c) 
    {
        FilterbankAnalysis::Setup sFilterbank;
        sFilterbank.coefficients.bufferSize = c.bufferSize;
        sFilterbank.coefficients.fftSize = c.fftSize;
        sFilterbank.coefficients.frameSize = c.fftSize;
        sFilterbank.coefficients.gain = 1.f;
        sFilterbank.coefficients.nChannels = 1;
        sFilterbank.parameters.windowType = sFilterbank.parameters.HANN_WINDOW;
        return sFilterbank;
    }

    int nBands;
};