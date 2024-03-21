#include "framework/framework.h"
#include "algorithm_library/spectrogram.h"
#include "filterbank/filterbank_wola.h"

// Spectrogram implemented using a weighted overlap-add (WOLA) filter bank. 
//
// author: Kristian Timm Andersen
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
        
        frame.resize(c.bufferSize);
        nBands = c.fftSize / 2 + 1;
        filterbankOut.resize(nBands);        
    }

    FilterbankAnalysisWOLA filterbank;
    DEFINE_MEMBER_ALGORITHMS(filterbank)

    void setWindow(I::Real window) 
    {
        auto pFilterbank = filterbank.getParameters();
        pFilterbank.windowType = pFilterbank.USER_DEFINED;
        filterbank.setParameters(pFilterbank);
        filterbank.setWindow(window);
    }

    static inline int getNFrames(int nSamples, int bufferSize) { return nSamples / bufferSize; }

private:

    void inline processOn(Input input, Output output)
    {
        for (auto nFrame = 0; nFrame < getNFrames(static_cast<int>(input.size()), C.bufferSize); nFrame++)
        {
            frame = input.segment(nFrame * C.bufferSize, C.bufferSize);
            filterbank.process(frame, filterbankOut);
            output.col(nFrame) = filterbankOut.abs2();
        }
    }

    size_t getDynamicSizeVariables() const final
    {
        return frame.getDynamicMemorySize() + filterbankOut.getDynamicMemorySize();
    }

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
    Eigen::ArrayXf frame;
    Eigen::ArrayXcf filterbankOut;

    friend AlgorithmImplementation<SpectrogramConfiguration, SpectrogramFilterbank>;
};