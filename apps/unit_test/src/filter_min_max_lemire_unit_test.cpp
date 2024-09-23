#include "filter_min_max/filter_min_max_lemire.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(FilterMinMaxLemire, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<FilterMinMaxLemire>()); }

TEST(FilterMaxLemire, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<FilterMaxLemire>()); }

TEST(FilterMinLemire, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<FilterMinLemire>()); }

TEST(StreamingMinMaxLemire, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<StreamingMinMaxLemire>()); }

TEST(StreamingMaxLemire, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<StreamingMaxLemire>()); }

TEST(StreamingMinLemire, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<StreamingMinLemire>()); }

// Description: Run StreamingMinMax once and compare to same execution broken into two process calls
TEST(FilterMinMaxLemire, StreamingMinMaxTwice)
{
    const int filterLength = 100;
    const int nChannels = 2;
    auto c = StreamingMinMaxConfiguration::Coefficients();
    c.nChannels = nChannels;
    c.filterLength = filterLength;
    StreamingMinMaxLemire streamingMinMax(c);

    const int nSamples = 1000;
    ArrayXXf input(nSamples, nChannels);
    input.setRandom();
    ArrayXXf outputSMMMin, outputSMMMax, outputSMMMin2, outputSMMMax2;
    std::tie(outputSMMMin, outputSMMMax) = streamingMinMax.initOutput(input);
    std::tie(outputSMMMin2, outputSMMMax2) = streamingMinMax.initOutput(input);

    // run two times with a reset between them
    streamingMinMax.process(input, {outputSMMMin, outputSMMMax});
    streamingMinMax.reset();
    streamingMinMax.process(input.topRows(nSamples / 2), {outputSMMMin2.topRows(nSamples / 2), outputSMMMax2.topRows(nSamples / 2)});
    streamingMinMax.process(input.bottomRows(nSamples / 2), {outputSMMMin2.bottomRows(nSamples / 2), outputSMMMax2.bottomRows(nSamples / 2)});

    EXPECT_EQ((outputSMMMax - outputSMMMax2).abs2().sum(), 0);
    EXPECT_EQ((outputSMMMin - outputSMMMin2).abs2().sum(), 0);
}

// Description: Run FilterMinMax two times and verify the output is the same
// Pass/Fail: FilterMinMax output is the same both times
TEST(FilterMinMaxLemire, RepeatFilterMinMaxprocess)
{
    const int filterLength = 100;
    const int nChannels = 2;
    auto c = FilterMinMaxConfiguration::Coefficients();
    c.nChannels = nChannels;
    c.filterLength = filterLength;
    FilterMinMaxLemire filterMinMax(c);

    const int nSamples = 1000;
    ArrayXXf input(nSamples, nChannels);
    input.setRandom();
    ArrayXXf outputFMMMin, outputFMMMax, outputFMMMin2, outputFMMMax2;
    std::tie(outputFMMMin, outputFMMMax) = filterMinMax.initOutput(input);
    std::tie(outputFMMMin2, outputFMMMax2) = filterMinMax.initOutput(input);

    // run two times with a reset between them
    filterMinMax.process(input, {outputFMMMin, outputFMMMax});
    filterMinMax.reset();
    filterMinMax.process(input, {outputFMMMin2, outputFMMMax2});

    EXPECT_EQ((outputFMMMax - outputFMMMax2).abs2().sum(), 0);
    EXPECT_EQ((outputFMMMin - outputFMMMin2).abs2().sum(), 0);
}

// Description: Run FilterMinMax and StreamingMinMax and verify the output is the same
// Pass/Fail: FilterMinMax output is the same both times
TEST(FilterMinMaxLemire, CompareFilterMinMaxandStreamingMinMaxprocess)
{
    const int filterLength = 100;
    const int nChannels = 2;
    auto cF = FilterMinMaxConfiguration::Coefficients();
    cF.nChannels = nChannels;
    cF.filterLength = filterLength;
    FilterMinMaxLemire filterMinMax(cF);

    auto cS = StreamingMinMaxConfiguration::Coefficients();
    cS.nChannels = nChannels;
    cS.filterLength = filterLength;
    StreamingMinMaxLemire streamingMinMax(cS);

    const int nSamples = 1000;
    ArrayXXf input(nSamples, nChannels);
    input.setRandom();
    ArrayXXf outputFMMMin, outputFMMMax, outputSMMMin, outputSMMMax;
    std::tie(outputFMMMin, outputFMMMax) = filterMinMax.initOutput(input);
    std::tie(outputSMMMin, outputSMMMax) = streamingMinMax.initOutput(input);

    filterMinMax.process(input, {outputFMMMin, outputFMMMax});
    streamingMinMax.process(input, {outputSMMMin, outputSMMMax});

    const int wHalf = (filterLength - 1) / 2;

    EXPECT_EQ((outputFMMMax.topRows(nSamples - wHalf) - outputSMMMax.bottomRows(nSamples - wHalf)).abs2().sum(), 0);
    EXPECT_EQ((outputFMMMin.topRows(nSamples - wHalf) - outputSMMMin.bottomRows(nSamples - wHalf)).abs2().sum(), 0);
}

// Description: Test output values of all streaming and filter min/max algorithms and compare them to a reference
// Pass/Fail: All algorithms should agree on min/max values
TEST(FilterMinMaxLemire, FilterStreamingMinMaxValuesTest)
{
    srand(static_cast<unsigned int>(time(NULL)));           // random seed to ensure we test a random signal
    const int filterLength = 99; // this has to be odd to ensure symmetric filter.
    const int nChannels = 2;

    auto cfMM = FilterMinMax::Coefficients();
    auto cfMin = FilterMin::Coefficients();
    auto cfMax = FilterMax::Coefficients();
    auto csMM = StreamingMinMax::Coefficients();
    auto csMin = StreamingMin::Coefficients();
    auto csMax = StreamingMax::Coefficients();

    cfMM.filterLength = filterLength;
    cfMin.filterLength = filterLength;
    cfMax.filterLength = filterLength;
    csMM.filterLength = filterLength;
    csMin.filterLength = filterLength;
    csMax.filterLength = filterLength;
    cfMM.nChannels = nChannels;
    cfMin.nChannels = nChannels;
    cfMax.nChannels = nChannels;
    csMM.nChannels = nChannels;
    csMin.nChannels = nChannels;
    csMax.nChannels = nChannels;

    FilterMinMax filterMinMax(cfMM);
    FilterMin filterMin(cfMin);
    FilterMax filterMax(cfMax);
    StreamingMinMax streamingMinMax(csMM);
    StreamingMin streamingMin(csMin);
    StreamingMax streamingMax(csMax);

    const int nSamples = 1000;
    ArrayXXf input(nSamples, nChannels);
    input.setRandom();

    ArrayXXf outputFMMMin, outputFMMMax, outputFMin, outputFMax, outputSMMMin, outputSMMMax, outputSMin, outputSMax;
    std::tie(outputFMMMin, outputFMMMax) = filterMinMax.initOutput(input);
    outputFMax = filterMax.initOutput(input);
    outputFMin = filterMin.initOutput(input);
    std::tie(outputSMMMin, outputSMMMax) = streamingMinMax.initOutput(input);
    outputSMax = streamingMax.initOutput(input);
    outputSMin = streamingMin.initOutput(input);

    ArrayXXf outputRef(nSamples + 2 * (filterLength - 1), nChannels);
    ArrayXXf minRef(nSamples + (filterLength - 1), nChannels);
    ArrayXXf maxRef(nSamples + (filterLength - 1), nChannels);
    outputRef.topRows(filterLength - 1).setZero();

    outputRef.bottomRows(filterLength - 1) = input.bottomRows<1>().replicate(filterLength - 1, 1);
    outputRef.middleRows(filterLength - 1, nSamples) = input;
    for (auto i = 0; i < nSamples + filterLength - 1; i++)
    {
        for (auto channel = 0; channel < nChannels; channel++)
        {
            minRef(i, channel) = *std::min_element(&outputRef(i, channel),
                                                   &outputRef(i, channel) + filterLength); //  + filterLength is put outside outputRef since otherwise we would go outside the
                                                                                           //  range of the vector and get Eigen warnings when running in debug mode
            maxRef(i, channel) = *std::max_element(&outputRef(i, channel),
                                                   &outputRef(i, channel) + filterLength); //  + filterLength is put outside outputRef since otherwise we would go outside the
                                                                                           //  range of the vector and get Eigen warnings when running in debug mode
        }
    }

    filterMinMax.process(input, {outputFMMMin, outputFMMMax});
    filterMin.process(input, outputFMin);
    filterMax.process(input, outputFMax);
    streamingMinMax.process(input, {outputSMMMin, outputSMMMax});
    streamingMax.process(input, outputSMax);
    streamingMin.process(input, outputSMin);

    EXPECT_EQ((outputSMin - minRef.topRows(nSamples)).abs2().sum(), 0);
    EXPECT_EQ((outputSMax - maxRef.topRows(nSamples)).abs2().sum(), 0);
    EXPECT_EQ((outputFMin - minRef.middleRows(filterLength / 2, nSamples)).abs2().sum(), 0);
    EXPECT_EQ((outputFMax - maxRef.middleRows(filterLength / 2, nSamples)).abs2().sum(), 0);
    EXPECT_EQ((outputSMMMin - minRef.topRows(nSamples)).abs2().sum(), 0);
    EXPECT_EQ((outputSMMMax - maxRef.topRows(nSamples)).abs2().sum(), 0);
    EXPECT_EQ((outputFMMMin - minRef.middleRows(filterLength / 2, nSamples)).abs2().sum(), 0);
    EXPECT_EQ((outputFMMMax - maxRef.middleRows(filterLength / 2, nSamples)).abs2().sum(), 0);
}
