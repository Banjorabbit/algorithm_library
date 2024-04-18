#include "filter_power_spectrum/calculate_filter_power_spectrum.h"
#include "framework/unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(FilterPowerSpectrum, Interface)
{
    EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<CalculateFilterPowerSpectrum>());
}

// test cascaded  power spectrum is equal to product of individual power spectrums
TEST(FilterPowerSpectrum, PowerFrequencyResponse)
{
    CalculateFilterPowerSpectrum filterSpectrum;
    
    int nSos = 10;
    Eigen::Array<float, 6, Eigen::Dynamic> sos(6, nSos);
    sos.setRandom();
    sos.row(3).setOnes();
    auto powerSpectrum = filterSpectrum.initOutput(sos);

    filterSpectrum.process(sos, powerSpectrum);

    Eigen::ArrayXf powerSpectrum2 = Eigen::ArrayXf::Ones(powerSpectrum.rows());
    for (auto i = 0; i < nSos; i++)
    {
        Eigen::ArrayXf newSpectrum(powerSpectrum2.rows());
        filterSpectrum.process(sos.col(i), newSpectrum);
        powerSpectrum2 *= newSpectrum;
    }
    
    float error = (powerSpectrum - powerSpectrum2).abs2().sum();
    fmt::print("Error: {}\n", error);
    EXPECT_TRUE(error < 1e-10f);
}