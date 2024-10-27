#include "filterbank_set/filterbank_set_wola.h"
#include "fmt/ranges.h"
#include "utilities/fastonebigheader.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(FilterbankSetAnalysis, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<FilterbankSetAnalysisWOLA>()); }

TEST(FilterbankSetSynthesis, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<FilterbankSetSynthesisWOLA>()); }

// process random signal through analysis and synthesis, and verify that all outputs are the same as the input
TEST(FilterbankSetAnalysis, Reconstruction)
{
    FilterbankSetAnalysisWOLA filterbank;
    FilterbankSetSynthesisWOLA inverseFilterbank;

    auto c = filterbank.getCoefficients();
    int nSamples = 10 * c.bufferSize;
    ArrayXf input = Eigen::ArrayXf::Random(nSamples);
    auto filterbankOut = filterbank.initOutput(input);
    auto output = inverseFilterbank.initOutput(filterbankOut);

    filterbank.process(input, filterbankOut);
    inverseFilterbank.process(filterbankOut, output);

    // delay is equal to FFT size - buffer size. For this type of filterbanks that corresponds to 2 * groupdelay - bufferSize
    std::vector<int> delays(c.nFilterbanks);
    for (auto i = 0; i < c.nFilterbanks; i++) 
    {
        delays[i] = static_cast<int>(std::round(filterbank.filterbanks[i].getDelaySamples())) * 2 - c.bufferSize / positivePow2(i); 
        Eigen::ArrayXf outputDelayCompensated = output.col(i).segment(delays[i], nSamples - delays[0]);
        float error = (outputDelayCompensated.head(nSamples - delays[0]) - input.head(nSamples - delays[0])).abs2().mean();
        fmt::print("Error {}: {}\n", i, error);
        EXPECT_TRUE(error < 1e-10f);
    }
}

// print delay
TEST(FilterbankSetAnalysis, MeasureDelay)
{
    FilterbankSetAnalysisWOLA filterbankSet;
    auto c = filterbankSet.getCoefficients();

    std::vector<int> delays(c.nFilterbanks);
    for (auto i = 0; i < c.nFilterbanks; i++)
    {
        delays[i] = 2 * static_cast<int>(std::round(filterbankSet.filterbanks[i].getDelaySamples())) - c.bufferSize / positivePow2(i);
    }

    fmt::print("Delays: {} samples\n", fmt::join(delays, ", "));
}