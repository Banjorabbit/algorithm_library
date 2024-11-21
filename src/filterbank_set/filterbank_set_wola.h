#pragma once
#include "algorithm_library/filterbank_set.h"
#include "filterbank/filterbank_simple.h"
#include "framework/framework.h"
#include "utilities/fastonebigheader.h"

class FilterbankSetAnalysisWOLA : public AlgorithmImplementation<FilterbankSetAnalysisConfiguration, FilterbankSetAnalysisWOLA>
{
  public:
    FilterbankSetAnalysisWOLA(Coefficients c = Coefficients()) : BaseAlgorithm{c}, filterbanks(initializeFilterbanks())
    {
        nBuffers.resize(C.nFilterbanks);
        bufferSizes.resize(C.nFilterbanks);
        for (auto i = 0; i < C.nFilterbanks; i++)
        {
            nBuffers[i] = positivePow2(i);
            bufferSizes[i] = C.bufferSize / nBuffers[i];
        }
    }

    VectorAlgo<FilterbankAnalysisSimple> filterbanks;
    DEFINE_MEMBER_ALGORITHMS(filterbanks)

  private:
    void processAlgorithm(Input input, Output output)
    {
        int nFrames = static_cast<int>(input.rows()) / C.bufferSize;
        assert(static_cast<float>(nFrames) == static_cast<float>(input.rows()) / C.bufferSize); // ensure length of input is an integer multiple of C.bufferSize

        for (auto iFrame = 0; iFrame < nFrames; iFrame++)
        {
            for (auto iFB = 0; iFB < C.nFilterbanks; iFB++)
            {
                for (auto iSubFrame = 0; iSubFrame < nBuffers[iFB]; iSubFrame++)
                {
                    filterbanks[iFB].process(input.segment(iFrame * C.bufferSize + iSubFrame * bufferSizes[iFB], bufferSizes[iFB]),
                                             output[iFB].col(iFrame * nBuffers[iFB] + iSubFrame));
                }
            }
        }

        // int i1 = 0;
        // int i2 = 0;
        // int i3 = 0;
        // for (auto i = 0; i < 7; i++)
        // {
        //     filterbanks[0].process(buffer.segment(i * C.bufferSize, C.bufferSize), output[0].col(i));

        //     if ((i & 1) == 0) // if last bit is 0, i.e. every 2nd time
        //     {
        //         filterbanks[1].process(buffer.segment((i + 1) * C.bufferSize, 2 * C.bufferSize), output[1].col(i1));
        //         i1 += 1;
        //     }
        //     if ((i & 3) == 0) // if last two bits are 0, i.e. every 4th time
        //     {
        //         filterbanks[2].process(buffer.segment((i + 3) * C.bufferSize, 4 * C.bufferSize), output[2].col(i2));
        //         i2 += 1;
        //     }

        //     if ((i & 7) == 0) // if last three bits are 0, i.e. every 8th time
        //     {
        //         filterbanks[3].process(buffer.segment((i + 7) * C.bufferSize, 8 * C.bufferSize), output[3].col(i3));
        //         i3 += 1;
        //     }
        // }

        // ------------------------------------------------------------------
        // filterbanks[0].process(input.segment(0 * C.bufferSize, C.bufferSize), output[0].col(7));
        // filterbanks[1].process(input.segment(0 * C.bufferSize, 2 * C.bufferSize), output[1].col(3));
        // filterbanks[2].process(input.segment(0 * C.bufferSize, 4 * C.bufferSize), output[2].col(1));
        // filterbanks[3].process(input.segment(0 * C.bufferSize, 8 * C.bufferSize), output[3].col(0));

        // ------------------------------------------------------------------
        // output[0].col(0).setZero();
        // output[1].col(0).setZero();
        // output[2].col(0).setZero();
        // filterbanks[3].process(input.segment(0 * C.bufferSize, 8 * C.bufferSize), output[3].col(0));

        // output[0].col(1).setZero();

        // output[0].col(2).setZero();
        // output[1].col(1).setZero();

        // output[0].col(3).setZero();

        // output[0].col(4).setZero();
        // output[1].col(2).setZero();
        // filterbanks[2].process(input.segment(0 * C.bufferSize, 4 * C.bufferSize), output[2].col(1));

        // output[0].col(5).setZero();

        // output[0].col(6).setZero();
        // filterbanks[1].process(input.segment(0 * C.bufferSize, 2 * C.bufferSize), output[1].col(3));

        // filterbanks[0].process(input.segment(0 * C.bufferSize, C.bufferSize), output[0].col(7));

        // filterbanks[0].process(input.segment(1 * C.bufferSize, C.bufferSize), output[0].col(8));
        // filterbanks[1].process(input.segment(2 * C.bufferSize, 2 * C.bufferSize), output[1].col(4));
        // filterbanks[2].process(input.segment(4 * C.bufferSize, 4 * C.bufferSize), output[2].col(3));
        // filterbanks[3].process(input.segment(8 * C.bufferSize, 8 * C.bufferSize), output[3].col(1));

        // filterbanks[0].process(input.segment(2 * C.bufferSize, C.bufferSize), output[0].col(9));

        // filterbanks[0].process(input.segment(3 * C.bufferSize, C.bufferSize), output[0].col(10));
        // filterbanks[1].process(input.segment(4 * C.bufferSize, 2 * C.bufferSize), output[1].col(5));

        // filterbanks[0].process(input.segment(4 * C.bufferSize, C.bufferSize), output[0].col(11));

        // filterbanks[0].process(input.segment(5 * C.bufferSize, C.bufferSize), output[0].col(12));
        // filterbanks[1].process(input.segment(6 * C.bufferSize, 2 * C.bufferSize), output[1].col(6));
        // filterbanks[2].process(input.segment(8 * C.bufferSize, 4 * C.bufferSize), output[2].col(4));

        // filterbanks[0].process(input.segment(6 * C.bufferSize, C.bufferSize), output[0].col(13));
    }

    size_t getDynamicSizeVariables() const final { return 2 * sizeof(int) * C.nFilterbanks; }

    std::vector<FilterbankAnalysisSimple::Coefficients> initializeFilterbanks()
    {
        std::vector<FilterbankAnalysisSimple::Coefficients> cFB(C.nFilterbanks);
        for (auto i = 0; i < C.nFilterbanks; i++)
        {
            cFB[i].bufferSize = C.bufferSize / positivePow2(i);
            cFB[i].filterbankType = cFB[i].HANN;
            cFB[i].nBands = FFTConfiguration::convertFFTSizeToNBands(4 * cFB[i].bufferSize);
            cFB[i].nChannels = 1;
        }
        return cFB;
    }

    std::vector<int> bufferSizes;
    std::vector<int> nBuffers;
    friend BaseAlgorithm;
};

class FilterbankSetSynthesisWOLA : public AlgorithmImplementation<FilterbankSetSynthesisConfiguration, FilterbankSetSynthesisWOLA>
{
  public:
    FilterbankSetSynthesisWOLA(Coefficients c = Coefficients()) : BaseAlgorithm{c}, inverseFilterbanks(initializeFilterbanks())
    {
        nBuffers.resize(C.nFilterbanks);
        bufferSizes.resize(C.nFilterbanks);
        for (auto i = 0; i < C.nFilterbanks; i++)
        {
            nBuffers[i] = positivePow2(i);
            bufferSizes[i] = C.bufferSize / nBuffers[i];
        }
    }

    VectorAlgo<FilterbankSynthesisSimple> inverseFilterbanks;
    DEFINE_MEMBER_ALGORITHMS(inverseFilterbanks)

  private:
    void processAlgorithm(Input input, Output output)
    {
        int nFrames = input[0].cols();
        assert(output.rows() == nFrames * C.bufferSize);
        assert(output.cols() == C.nFilterbanks);

        for (auto iFrame = 0; iFrame < nFrames; iFrame++)
        {
            for (auto iFB = 0; iFB < C.nFilterbanks; iFB++)
            {
                for (auto iSubFrame = 0; iSubFrame < nBuffers[iFB]; iSubFrame++)
                {
                    inverseFilterbanks[iFB].process(input[iFB].col(iFrame * nBuffers[iFB] + iSubFrame),
                                                    output.col(iFB).segment(iFrame * C.bufferSize + iSubFrame * bufferSizes[iFB], bufferSizes[iFB]));
                }
            }
        }
    }

    std::vector<FilterbankSynthesisSimple::Coefficients> initializeFilterbanks()
    {
        std::vector<FilterbankSynthesisSimple::Coefficients> cFB(C.nFilterbanks);
        for (auto i = 0; i < C.nFilterbanks; i++)
        {
            cFB[i].bufferSize = C.bufferSize / positivePow2(i);
            cFB[i].filterbankType = cFB[i].HANN;
            cFB[i].nBands = FFTConfiguration::convertFFTSizeToNBands(4 * cFB[i].bufferSize);
            cFB[i].nChannels = 1;
        }
        return cFB;
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = 2 * sizeof(int) * C.nFilterbanks; // dynamic size of bufferSizes
        return size;
    }

    std::vector<int> bufferSizes;
    std::vector<int> nBuffers;

    friend BaseAlgorithm;
};