#include "min_phase_spectrum/min_phase_spectrum_cepstrum.h"

DEFINE_SOURCE_INTERFACE(MinPhaseSpectrumConfiguration, MinPhaseSpectrumCepstrum)

MinPhaseSpectrum::MinPhaseSpectrum(const Coefficients& c) : Algorithm<MinPhaseSpectrumConfiguration>{ c } {}