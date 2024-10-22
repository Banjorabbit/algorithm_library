#pragma once
#include "algorithm_library/filterbank_set.h"
#include "filterbank/filterbank_wola.h"
#include "framework/framework.h"

class FilterbankSetAnalysisWOLA : public AlgorithmImplementation<FilterbankSetAnalysisConfiguration, FilterbankSetAnalysisWOLA>
{
  public:
    FilterbankSetAnalysisWOLA(Coefficients c = Coefficients()) : BaseAlgorithm{c}, filterbanks(initializeFilterbanks(c))
    {
        bufferSizes.resize(c.nFilterbanks);
        for (auto i = 0; i < c.nFilterbanks; i++)
        {
            bufferSizes[i] = c.bufferSize * (1 << i);
        }
    }

    VectorAlgo<FilterbankAnalysisWOLA> filterbanks;
    DEFINE_MEMBER_ALGORITHMS(filterbanks)

  private:
    void processAlgorithm(Input input, Output output)
    {
        for (auto iFB = 0; iFB < C.nFilterbanks; iFB++)
        {
            const int nFrames = input.size() / bufferSizes[iFB];
            const int bufferSize = bufferSizes[iFB];
            for (auto iFrame = 0; iFrame < nFrames; iFrame++)
            {
                filterbanks[iFB].process(input.segment(iFrame * bufferSize, bufferSize), output[iFB].col(iFrame));
            }
        }
    }

    std::vector<FilterbankAnalysisWOLA::Coefficients> initializeFilterbanks(const Coefficients &c)
    {
        std::vector<FilterbankAnalysisWOLA::Coefficients> cFB(c.nFilterbanks);
        for (auto i = 0; i < c.nFilterbanks; i++)
        {
            cFB[i].bufferSize = c.bufferSize * (1 << i);
            cFB[i].filterbankType = cFB[i].HANN;
            cFB[i].nBands = FFTConfiguration::convertFFTSizeToNBands(4 * cFB[i].bufferSize);
            cFB[i].nChannels = 1;
        }
        return cFB;
    }

    size_t getDynamicSizeVariables() const final { return sizeof(int) * C.nFilterbanks; } // dynamic size of bufferSizes

    std::vector<int> bufferSizes;

    friend BaseAlgorithm;
};
