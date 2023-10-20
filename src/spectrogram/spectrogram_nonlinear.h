#include "framework/framework.h"
#include "algorithm_library/spectrogram.h"
#include "filterbank/filterbank_wola.h"

class SpectrogramNonlinear: public IAlgorithm<SpectrogramConfiguration, SpectrogramNonlinear>
{
public:
    SpectrogramNonlinear(Coefficients c = Coefficients()) : 
        IAlgorithm<SpectrogramConfiguration, SpectrogramNonlinear>{c},
        filterbank0(convertCoefficientsToFilterbankCoefficients(c)),
        filterbank1(convertCoefficientsToFilterbankCoefficients(c)),
        filterbank2(convertCoefficientsToFilterbankCoefficients(c))
    { 
        auto pFilterbank = filterbank0.getParameters();
        pFilterbank.windowType = pFilterbank.USER_DEFINED;
        filterbank0.setParameters(pFilterbank);
        filterbank1.setParameters(pFilterbank);
        filterbank2.setParameters(pFilterbank);
        // set windows
        nBands = C.fftSize / 2 + 1;
        Eigen::ArrayXf window = hann(C.fftSize);
        Eigen::ArrayXf windowSmall = hann(C.bufferSize);

        Eigen::ArrayXf windowNormalized = window / std::sqrt(window.abs2().sum());
        filterbank0.setWindow(windowNormalized);

        window.segment(C.fftSize/2, C.bufferSize / 2) = windowSmall.tail(C.bufferSize / 2);
        window.tail((C.fftSize - C.bufferSize) / 2).setZero();
        windowNormalized = window / std::sqrt(window.abs2().sum());
        filterbank1.setWindow(windowNormalized);

        window = hann(C.fftSize);
        window.head((C.fftSize - C.bufferSize) / 2).setZero();
        window.segment((C.fftSize - C.bufferSize) / 2, C.bufferSize / 2) = windowSmall.head(C.bufferSize / 2);
        windowNormalized = window / std::sqrt(window.abs2().sum());
        filterbank2.setWindow(windowNormalized);
    }

    FilterbankAnalysisWOLA filterbank0;
    FilterbankAnalysisWOLA filterbank1;
    FilterbankAnalysisWOLA filterbank2;
    DEFINE_MEMBER_ALGORITHMS(filterbank0, filterbank1, filterbank2)

    void inline processOn(Input input, Output output)
    {
        for (auto nFrame = 0; nFrame < getNFrames(input.size(), C.bufferSize); nFrame++)
        {
            Eigen::ArrayXf frame = input.segment(nFrame * C.bufferSize, C.bufferSize);
            Eigen::ArrayXcf filterbankOut0(nBands), filterbankOut1(nBands), filterbankOut2(nBands);
            filterbank0.process(frame, filterbankOut0);
            filterbank1.process(frame, filterbankOut1);
            filterbank2.process(frame, filterbankOut2);
            output.col(nFrame) = filterbankOut0.abs2().min(filterbankOut1.abs2()).min(filterbankOut2.abs2());
        }
    }

    void setWindow(I::Real window) 
    {
        // calculate windows
        const int ratio = C.fftSize / C.bufferSize; 
        const int n = window.size();
        const int n2 = n / 2;
        const int nSmall = n2 / ratio;
        Eigen::ArrayXf window0 = window;
        window0 /= std::sqrt(window0.abs2().sum());
        
        Eigen::ArrayXf window1(n);
        window1.head(n2) = window.head(n2);
        window1.segment(n2, nSmall) = Eigen::Map<const Eigen::ArrayXf, 0, Eigen::InnerStride<>>(window.data() + n2, nSmall, 1, Eigen::InnerStride<>(ratio));
        window1.tail(n2 - nSmall).setZero();
        window1 /= std::sqrt(window1.abs2().sum());

        Eigen::ArrayXf window2(n);
        window2.head(n2 - nSmall).setZero();
        window2.segment(n2 - nSmall, nSmall) = Eigen::Map<const Eigen::ArrayXf, 0, Eigen::InnerStride<>>(window.data(), nSmall, 1, Eigen::InnerStride<>(ratio));
        window2.tail(n2) = window.tail(n2);
        window2 /= std::sqrt(window2.abs2().sum());

        // set windows in filterbanks
        auto pF0 = filterbank0.getParameters();
        pF0.windowType = pF0.USER_DEFINED;
        filterbank0.setParameters(pF0);
        filterbank0.setWindow(window0);

        auto pF1 = filterbank1.getParameters();
        pF1.windowType = pF1.USER_DEFINED;
        filterbank1.setParameters(pF1);
        filterbank1.setWindow(window1);

        auto pF2 = filterbank2.getParameters();
        pF2.windowType = pF2.USER_DEFINED;
        filterbank2.setParameters(pF2);
        filterbank2.setWindow(window2);
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