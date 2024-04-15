#include "critical_bands/critical_bands_bark.h"


DEFINE_ALGORITHM_CONSTRUCTOR(CriticalBandsMax, CriticalBandsBarkMax, CriticalBandsMaxConfiguration)
DEFINE_ALGORITHM_CONSTRUCTOR(CriticalBandsSum, CriticalBandsBarkSum, CriticalBandsSumConfiguration)
DEFINE_ALGORITHM_CONSTRUCTOR(CriticalBandsMean, CriticalBandsBarkMean, CriticalBandsMeanConfiguration)

namespace 
{
    constexpr std::array<float, 25> frequencyCornersArrayHz = { 20.f, 100.f, 200.f, 300.f, 400.f, 510.f, 630.f, 770.f, 920.f, 1080.f, 1270.f, 1480.f, 1720.f, 2000.f, 2320.f, 2700.f, 3150.f, 3700.f, 4400.f, 5300.f, 6400.f, 7700.f, 9500.f, 12000.f, 15500.f };
    constexpr std::array<float, 24> frequencyCentersArrayHz = { 50.f,150.f,250.f,350.f,450.f,570.f,700.f,840.f,1000.f,1170.f,1370.f,1600.f,1850.f,2150.f,2500.f,2900.f,3400.f,4000.f,4800.f,5800.f,7000.f, 8500.f, 10500.f, 13500.f }; 
}

int CriticalBandsConfiguration::getNCriticalBands(float sampleRate)
{
    // calculate number of critical bands
    Eigen::Map<const Eigen::ArrayXf> frequencyCornersHz(frequencyCornersArrayHz.data(), frequencyCornersArrayHz.size());
    return (frequencyCornersHz <= sampleRate / 2).cast<int>().sum() - 1;
}

Eigen::ArrayXf CriticalBandsConfiguration::getCenterFrequencies(float sampleRate) 
{ 
    return Eigen::Map<const Eigen::ArrayXf>(frequencyCentersArrayHz.data(), getNCriticalBands(sampleRate)); 
}

Eigen::ArrayXf CriticalBandsConfiguration::getCornerFrequencies(float sampleRate) 
{ 
    return Eigen::Map<const Eigen::ArrayXf>(frequencyCornersArrayHz.data(), getNCriticalBands(sampleRate) + 1); 
}

void CriticalBandsSum::inverse(I::Real2D xPower, O::Real2D yPower) { static_cast<CriticalBandsBarkSumSingleBufferImpl*>(pimpl.get())->algo.inverse(xPower, yPower); }
void CriticalBandsMean::inverse(I::Real2D xPower, O::Real2D yPower) { static_cast<CriticalBandsBarkMeanSingleBufferImpl*>(pimpl.get())->algo.inverse(xPower, yPower); }
void CriticalBandsMax::inverse(I::Real2D xPower, O::Real2D yPower) { static_cast<CriticalBandsBarkMaxSingleBufferImpl*>(pimpl.get())->algo.inverse(xPower, yPower); }

