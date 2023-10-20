#include "critical_bands/critical_bands_bark.h"

int CriticalBandsConfiguration::getNCriticalBands(float sampleRate) { return BarkScale::getNCriticalBands(sampleRate); }

template<> 
Algorithm<CriticalBandsSumConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<CriticalBandsBarkSum, CriticalBandsSumConfiguration>>();
} 

template<> 
Algorithm<CriticalBandsSumConfiguration>::~Algorithm() {} 

template<>
Algorithm<CriticalBandsSumConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<CriticalBandsBarkSum, CriticalBandsSumConfiguration>>(c);
} 

template<> 
Algorithm<CriticalBandsMeanConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<CriticalBandsBarkMean, CriticalBandsMeanConfiguration>>();
} 

template<> 
Algorithm<CriticalBandsMeanConfiguration>::~Algorithm() {} 

template<>
Algorithm<CriticalBandsMeanConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<CriticalBandsBarkMean, CriticalBandsMeanConfiguration>>(c);
} 

template<> 
Algorithm<CriticalBandsMaxConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<CriticalBandsBarkMax, CriticalBandsMaxConfiguration>>();
} 

template<> 
Algorithm<CriticalBandsMaxConfiguration>::~Algorithm() {} 

template<>
Algorithm<CriticalBandsMaxConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<CriticalBandsBarkMax, CriticalBandsMaxConfiguration>>(c);
} 


CriticalBandsSum::CriticalBandsSum(const Coefficients& c) : Algorithm<CriticalBandsSumConfiguration>{ c } {}
int CriticalBandsSum::getNCriticalBands(float sampleRate) { return BarkScale::getNCriticalBands(sampleRate); }
Eigen::ArrayXf CriticalBandsSum::getCenterFrequencies(float sampleRate) { return BarkScale::getCenterFrequencies(sampleRate); }
Eigen::ArrayXf CriticalBandsSum::getCornerFrequencies(float sampleRate) { return BarkScale::getCornerFrequencies(sampleRate); }
void CriticalBandsSum::inverse(I::Real2D xPower, O::Real2D yPower) { static_cast<Impl<CriticalBandsSum, CriticalBandsSumConfiguration>*>(pimpl.get())->algo.inverse(xPower, yPower); }

CriticalBandsMean::CriticalBandsMean(const Coefficients& c) : Algorithm<CriticalBandsMeanConfiguration>{ c } {}
int CriticalBandsMean::getNCriticalBands(float sampleRate) { return BarkScale::getNCriticalBands(sampleRate); }
Eigen::ArrayXf CriticalBandsMean::getCenterFrequencies(float sampleRate) { return BarkScale::getCenterFrequencies(sampleRate); }
Eigen::ArrayXf CriticalBandsMean::getCornerFrequencies(float sampleRate) { return BarkScale::getCornerFrequencies(sampleRate); }
void CriticalBandsMean::inverse(I::Real2D xPower, O::Real2D yPower) { static_cast<Impl<CriticalBandsMean, CriticalBandsMeanConfiguration>*>(pimpl.get())->algo.inverse(xPower, yPower); }

CriticalBandsMax::CriticalBandsMax(const Coefficients& c) : Algorithm<CriticalBandsMaxConfiguration>{ c } {}
int CriticalBandsMax::getNCriticalBands(float sampleRate) { return BarkScale::getNCriticalBands(sampleRate); }
Eigen::ArrayXf CriticalBandsMax::getCenterFrequencies(float sampleRate) { return BarkScale::getCenterFrequencies(sampleRate); }
Eigen::ArrayXf CriticalBandsMax::getCornerFrequencies(float sampleRate) { return BarkScale::getCornerFrequencies(sampleRate); }
void CriticalBandsMax::inverse(I::Real2D xPower, O::Real2D yPower) { static_cast<Impl<CriticalBandsMax, CriticalBandsMaxConfiguration>*>(pimpl.get())->algo.inverse(xPower, yPower); }