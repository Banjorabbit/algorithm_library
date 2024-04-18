#include "algorithm_library/spectrogram.h"
#include "filterbank/filterbank_wola.h"
#include "framework/framework.h"

// Spectrogram implemented using a weighted overlap-add (WOLA) filter bank.
//
// author: Kristian Timm Andersen
class SpectrogramFilterbank : public AlgorithmImplementation<SpectrogramConfiguration, SpectrogramFilterbank>
{
  public:
    SpectrogramFilterbank(Coefficients c = Coefficients()) : BaseAlgorithm{c}, filterbank(convertToFilterbankCoefficients(c)) { filterbankOut.resize(c.nBands); }

    FilterbankAnalysisWOLA filterbank;
    DEFINE_MEMBER_ALGORITHMS(filterbank)

  private:
    void inline processAlgorithm(Input input, Output output)
    {
        for (auto nFrame = 0; nFrame < Configuration::getNFrames(input.size(), C.bufferSize); nFrame++)
        {
            filterbank.process(input.segment(nFrame * C.bufferSize, C.bufferSize), filterbankOut);
            output.col(nFrame) = filterbankOut.abs2();
        }
    }

    size_t getDynamicSizeVariables() const final { return filterbankOut.getDynamicMemorySize(); }

    FilterbankAnalysisWOLA::Coefficients convertToFilterbankCoefficients(const Coefficients &c)
    {
        FilterbankAnalysisWOLA::Coefficients cFilterbank;
        cFilterbank.bufferSize = c.bufferSize;
        cFilterbank.nChannels = 1;
        cFilterbank.nBands = c.nBands;
        switch (c.algorithmType)
        {
        default: // Hann is the default case
        case Coefficients::HANN: cFilterbank.filterbankType = cFilterbank.HANN; break;
        case Coefficients::SQRT_HANN: cFilterbank.filterbankType = cFilterbank.SQRT_HANN; break;
        case Coefficients::WOLA: cFilterbank.filterbankType = cFilterbank.WOLA; break;
        }
        return cFilterbank;
    }

    Eigen::ArrayXcf filterbankOut;

    friend BaseAlgorithm;
};