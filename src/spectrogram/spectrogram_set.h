#include "algorithm_library/spectrogram.h"
#include "filterbank_set/filterbank_set_wola.h"
#include "framework/framework.h"
#include "spectrogram/upscale2d_linear.h"
#include "utilities/fastonebigheader.h"

// Spectrogram implemented as a combination of a set of filterbanks spectrograms. The criteria used for selecting the best time/frequency bin is the minimum power.
//
// author: Kristian Timm Andersen
class SpectrogramSet : public AlgorithmImplementation<SpectrogramConfiguration, SpectrogramSet>
{
  public:
    SpectrogramSet(Coefficients c = {.bufferSize = 1024, .nBands = 1025, .algorithmType = Coefficients::ADAPTIVE_HANN_8})
        : BaseAlgorithm{c}, filterbankSet(initializeFilterbankSet(c))
    {
        assert((c.algorithmType == Coefficients::ADAPTIVE_HANN_8) || (c.algorithmType == Coefficients::ADAPTIVE_WOLA_8));
        Eigen::ArrayXf inputFrame(c.bufferSize);
        filterbankOut = filterbankSet.initOutput(inputFrame);

        auto cUpscale = upscale.getCoefficients();
        cUpscale.resize(nFilterbanks);
        for (auto i = 0; i < nFilterbanks; i++)
        {
            cUpscale[i].factorHorizontal = positivePow2(nFilterbanks - 1 - i);
            cUpscale[i].factorVertical = positivePow2(i);
        }
        upscale.setCoefficients(cUpscale);
    }

    FilterbankSetAnalysisWOLA filterbankSet;
    VectorAlgo<Upscale2DLinear> upscale;
    DEFINE_MEMBER_ALGORITHMS(filterbankSet, upscale)

  private:
    FilterbankSetAnalysisWOLA::Coefficients initializeFilterbankSet(const Coefficients &c)
    {
        FilterbankSetAnalysisWOLA::Coefficients cFilterbank;
        cFilterbank.bufferSize = c.bufferSize;
        cFilterbank.nBands = c.nBands;
        cFilterbank.nFilterbanks = nFilterbanks; // hardcoded to 4, which corresponds to a division of 2^(4-1) = 8
        cFilterbank.filterbankType =
            (c.algorithmType == Coefficients::ADAPTIVE_WOLA_8) ? FilterbankSetAnalysisWOLA::Coefficients::WOLA : FilterbankSetAnalysisWOLA::Coefficients::HANN;
        return cFilterbank;
    }

    void inline processAlgorithm(Input input, Output output)
    {
        std::vector<Eigen::ArrayXXcf> filterbankOut = filterbankSet.initOutput(input);
        filterbankSet.process(input, filterbankOut);

        Eigen::ArrayXXf filterbankPower = filterbankOut[0].abs2();
        upscale[0].process(filterbankPower, output);
        for (auto iFB = 1; iFB < static_cast<int>(filterbankOut.size()); iFB++)
        {
            filterbankPower = filterbankOut[iFB].abs2();
            Eigen::ArrayXXf powerUpscaled = upscale[iFB].initOutput(filterbankPower);
            upscale[iFB].process(filterbankPower, powerUpscaled);
            output = output.min(powerUpscaled);
        }
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = 0;
        for (auto i = 0; i < static_cast<int>(filterbankOut.size()); i++)
        {
            size += filterbankOut[i].getDynamicMemorySize();
        }
        return size;
    }

    static constexpr int nFilterbanks = 4;
    std::vector<Eigen::ArrayXXcf> filterbankOut;

    friend BaseAlgorithm;
};
