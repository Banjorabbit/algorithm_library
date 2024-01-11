#include "critical_bands/critical_bands_bark.h"

int CriticalBandsConfiguration::getNCriticalBands(float sampleRate) { return BarkScale::getNCriticalBands(sampleRate); }

DEFINE_ALGORITHM_CONSTRUCTOR(CriticalBandsMax, CriticalBandsBarkMax, CriticalBandsMaxConfiguration)
DEFINE_ALGORITHM_CONSTRUCTOR(CriticalBandsSum, CriticalBandsBarkSum, CriticalBandsSumConfiguration)
DEFINE_ALGORITHM_CONSTRUCTOR(CriticalBandsMean, CriticalBandsBarkMean, CriticalBandsMeanConfiguration)

int CriticalBandsSum::getNCriticalBands(float sampleRate) { return BarkScale::getNCriticalBands(sampleRate); }
Eigen::ArrayXf CriticalBandsSum::getCenterFrequencies(float sampleRate) { return BarkScale::getCenterFrequencies(sampleRate); }
Eigen::ArrayXf CriticalBandsSum::getCornerFrequencies(float sampleRate) { return BarkScale::getCornerFrequencies(sampleRate); }
void CriticalBandsSum::inverse(I::Real2D xPower, O::Real2D yPower) { static_cast<CriticalBandsBarkSumSingleBufferImpl*>(pimpl.get())->algo.inverse(xPower, yPower); }

int CriticalBandsMean::getNCriticalBands(float sampleRate) { return BarkScale::getNCriticalBands(sampleRate); }
Eigen::ArrayXf CriticalBandsMean::getCenterFrequencies(float sampleRate) { return BarkScale::getCenterFrequencies(sampleRate); }
Eigen::ArrayXf CriticalBandsMean::getCornerFrequencies(float sampleRate) { return BarkScale::getCornerFrequencies(sampleRate); }
void CriticalBandsMean::inverse(I::Real2D xPower, O::Real2D yPower) { static_cast<CriticalBandsBarkMeanSingleBufferImpl*>(pimpl.get())->algo.inverse(xPower, yPower); }

int CriticalBandsMax::getNCriticalBands(float sampleRate) { return BarkScale::getNCriticalBands(sampleRate); }
Eigen::ArrayXf CriticalBandsMax::getCenterFrequencies(float sampleRate) { return BarkScale::getCenterFrequencies(sampleRate); }
Eigen::ArrayXf CriticalBandsMax::getCornerFrequencies(float sampleRate) { return BarkScale::getCornerFrequencies(sampleRate); }
void CriticalBandsMax::inverse(I::Real2D xPower, O::Real2D yPower) { static_cast<CriticalBandsBarkMaxSingleBufferImpl*>(pimpl.get())->algo.inverse(xPower, yPower); }