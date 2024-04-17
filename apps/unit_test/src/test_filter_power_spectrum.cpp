#include "filter_power_spectrum/calculate_filter_power_spectrum.h"
#include "framework/unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(FilterPowerSpectrum, Interface)
{
    EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<CalculateFilterPowerSpectrum>());
}
