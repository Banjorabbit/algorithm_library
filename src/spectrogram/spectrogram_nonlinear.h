#include "algorithm_library/spectrogram.h"
#include "filterbank/filterbank_wola.h"
#include "framework/framework.h"

// Spectrogram implemented as a nonlinear combination of several standard spectrograms. The criteria used for selecting the best time/frequency bin is the minimum power.
//
// author: Kristian Timm Andersen
class SpectrogramNonlinear : public AlgorithmImplementation<SpectrogramConfiguration, SpectrogramNonlinear>
{
  public:
    SpectrogramNonlinear(Coefficients c = Coefficients())
        : BaseAlgorithm{c},
          filterbank0({.nChannels = 1, .bufferSize = c.bufferSize, .nBands = c.nBands, .filterbankType = FilterbankAnalysisWOLA::Coefficients::USER_DEFINED}),
          filterbank1({.nChannels = 1, .bufferSize = c.bufferSize, .nBands = c.nBands, .filterbankType = FilterbankAnalysisWOLA::Coefficients::USER_DEFINED}),
          filterbank2({.nChannels = 1, .bufferSize = c.bufferSize, .nBands = c.nBands, .filterbankType = FilterbankAnalysisWOLA::Coefficients::USER_DEFINED})
    {
        // set windows
        int frameSize = filterbank0.getFrameSize();
        Eigen::ArrayXf window = hann(frameSize);
        float sqrtPower = std::sqrt(window.abs2().sum());
        Eigen::ArrayXf windowSmall = hann(c.bufferSize);

        filterbank0.setWindow(window);

        window.setZero();
        window.segment(frameSize / 2 - c.bufferSize, 2 * c.bufferSize) = hann(2 * c.bufferSize);
        window = window * sqrtPower / std::sqrt(window.abs2().sum());
        filterbank1.setWindow(window);

        window.segment(frameSize / 2 - 2 * c.bufferSize, 4 * c.bufferSize) = hann(4 * c.bufferSize);
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