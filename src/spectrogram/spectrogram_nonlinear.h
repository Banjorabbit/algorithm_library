#include "algorithm_library/spectrogram.h"
#include "filterbank/filterbank_wola.h"
#include "framework/framework.h"

// Spectrogram implemented as a nonlinear combination of several standard spectrograms. The criteria used for selecting the best time/frequency bin is the minimum power.
//
// author: Kristian Timm Andersen
class SpectrogramNonlinear : public AlgorithmImplementation<SpectrogramConfiguration, SpectrogramNonlinear>
{
  public:
    SpectrogramNonlinear(Coefficients c = {.bufferSize = 128, .nBands = 513, .algorithmType = Coefficients::ADAPTIVE_HANN})
        : BaseAlgorithm{c}, filterbank0({initializeFilterbanks(c)}), filterbank1({initializeFilterbanks(c)}), filterbank2({initializeFilterbanks(c)})
    {
        // set windows
        int frameSize = filterbank0.getFrameSize();
        Eigen::ArrayXf window = hann(frameSize);
        float sqrtPower = std::sqrt(window.abs2().sum());
        filterbank0.setWindow(window);

        window.setZero();
        Eigen::ArrayXf windowSmall = hann(2 * c.bufferSize);
        window.segment(frameSize / 2 - c.bufferSize, 2 * c.bufferSize) = windowSmall;
        window = window * sqrtPower / std::sqrt(window.abs2().sum());
        filterbank1.setWindow(window);

        int frameSize2 = std::max(FFTConfiguration::getValidFFTSize(frameSize / 2), 2 * c.bufferSize); // half of large frame size
        window.setZero();
        window.segment(frameSize / 2 - frameSize2 / 2, frameSize2) = hann(frameSize2);
        window = window * sqrtPower / std::sqrt(window.abs2().sum());
        filterbank2.setWindow(window);

        filterbankOut.resize(c.nBands);
        frame.resize(c.bufferSize);
    }

    FilterbankAnalysisWOLA filterbank0;
    FilterbankAnalysisWOLA filterbank1;
    FilterbankAnalysisWOLA filterbank2;
    DEFINE_MEMBER_ALGORITHMS(filterbank0, filterbank1, filterbank2)

  private:
    FilterbankAnalysisWOLA::Coefficients initializeFilterbanks(const Coefficients &c)
    {
        auto cFilterbank = FilterbankAnalysisWOLA::Coefficients();
        cFilterbank.nChannels = 1;
        cFilterbank.bufferSize = c.bufferSize;
        cFilterbank.nBands = c.nBands;
        if (c.algorithmType == Coefficients::ADAPTIVE_HANN)
        {
            cFilterbank.filterbankType = FilterbankAnalysisWOLA::Coefficients::HANN; // sets correct window size, but values are overwritten in constructor
        }
        else if (c.algorithmType == Coefficients::ADAPTIVE_WOLA)
        {
            cFilterbank.filterbankType = FilterbankAnalysisWOLA::Coefficients::WOLA; // sets correct window size, but values are overwritten in constructor
        }
        return cFilterbank;
    }

    void inline processAlgorithm(Input input, Output output)
    {
        for (auto nFrame = 0; nFrame < Configuration::getNFrames(static_cast<int>(input.size()), C.bufferSize); nFrame++)
        {
            frame = input.segment(nFrame * C.bufferSize, C.bufferSize);

            filterbank0.process(frame, filterbankOut);
            output.col(nFrame) = filterbankOut.abs2();

            filterbank1.process(frame, filterbankOut);
            output.col(nFrame) = output.col(nFrame).min(filterbankOut.abs2());

            filterbank2.process(frame, filterbankOut);
            output.col(nFrame) = output.col(nFrame).min(filterbankOut.abs2());
        }
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = filterbankOut.getDynamicMemorySize();
        size += frame.getDynamicMemorySize();
        return size;
    }

    Eigen::ArrayXcf filterbankOut;
    Eigen::ArrayXf frame;

    friend BaseAlgorithm;
};