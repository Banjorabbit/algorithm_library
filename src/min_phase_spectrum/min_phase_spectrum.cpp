#include "min_phase_spectrum/min_phase_spectrum_cepstrum.h"

template<> 
Algorithm<MinPhaseSpectrumConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<MinPhaseSpectrumCepstrum, MinPhaseSpectrumConfiguration>>();
} 

template<> 
Algorithm<MinPhaseSpectrumConfiguration>::~Algorithm() {} 

template<>
Algorithm<MinPhaseSpectrumConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<MinPhaseSpectrumCepstrum, MinPhaseSpectrumConfiguration>>(c);
} 

MinPhaseSpectrum::MinPhaseSpectrum(const Coefficients& c) : Algorithm<MinPhaseSpectrumConfiguration>{ c } {}