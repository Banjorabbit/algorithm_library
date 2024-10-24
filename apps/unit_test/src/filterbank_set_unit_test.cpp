#include "filterbank_set/filterbank_set_wola.h"
#include "fmt/ranges.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(FilterbankSetAnalysis, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<FilterbankSetAnalysisWOLA>()); }

TEST(FilterbankSetAnalysis, MeasureDelay)
{
    FilterbankSetAnalysisWOLA filterbankSet;

    auto c = filterbankSet.getCoefficients();

    std::vector<int> delays(c.nFilterbanks);
    for (auto i = 0; i < c.nFilterbanks; i++)
    {
        delays[i] = static_cast<int>(std::round(filterbankSet.filterbanks[i].getDelaySamples()));
    }

    fmt::print("Delays: {} samples\n", fmt::join(delays, ", "));
}