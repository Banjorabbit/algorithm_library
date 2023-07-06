#include "critical_bands/critical_bands_bark.h"

DEFINE_SOURCE_INTERFACE(CriticalBandsSumConfiguration, CriticalBandsBarkSum)
DEFINE_SOURCE_INTERFACE(CriticalBandsMeanConfiguration, CriticalBandsBarkMean)
DEFINE_SOURCE_INTERFACE(CriticalBandsMaxConfiguration, CriticalBandsBarkMax)

CriticalBandsSum::CriticalBandsSum(const Coefficients& c) : Algorithm<CriticalBandsSumConfiguration>{ c } {}
int CriticalBandsSum::getNCriticalBands(float sampleRate) { return BarkScale::getNCriticalBands(sampleRate); }
Eigen::ArrayXf CriticalBandsSum::getCenterFrequencies(float sampleRate) { return BarkScale::getCenterFrequencies(sampleRate); }
Eigen::ArrayXf CriticalBandsSum::getCornerFrequencies(float sampleRate) { return BarkScale::getCornerFrequencies(sampleRate); }
void CriticalBandsSum::inverse(I::Real2D xPower, O::Real2D yPower) { pimpl->algo.inverse(xPower, yPower); }

CriticalBandsMean::CriticalBandsMean(const Coefficients& c) : Algorithm<CriticalBandsMeanConfiguration>{ c } {}
int CriticalBandsMean::getNCriticalBands(float sampleRate) { return BarkScale::getNCriticalBands(sampleRate); }
Eigen::ArrayXf CriticalBandsMean::getCenterFrequencies(float sampleRate) { return BarkScale::getCenterFrequencies(sampleRate); }
Eigen::ArrayXf CriticalBandsMean::getCornerFrequencies(float sampleRate) { return BarkScale::getCornerFrequencies(sampleRate); }
void CriticalBandsMean::inverse(I::Real2D xPower, O::Real2D yPower) { pimpl->algo.inverse(xPower, yPower); }

CriticalBandsMax::CriticalBandsMax(const Coefficients& c) : Algorithm<CriticalBandsMaxConfiguration>{ c } {}
int CriticalBandsMax::getNCriticalBands(float sampleRate) { return BarkScale::getNCriticalBands(sampleRate); }
Eigen::ArrayXf CriticalBandsMax::getCenterFrequencies(float sampleRate) { return BarkScale::getCenterFrequencies(sampleRate); }
Eigen::ArrayXf CriticalBandsMax::getCornerFrequencies(float sampleRate) { return BarkScale::getCornerFrequencies(sampleRate); }
void CriticalBandsMax::inverse(I::Real2D xPower, O::Real2D yPower) { pimpl->algo.inverse(xPower, yPower); }