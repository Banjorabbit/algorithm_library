#include "spectrogram/spectrogram_filterbank.h"
#include "spectrogram/spectrogram_set.h"

using FilterbankImpl = Implementation<SpectrogramFilterbank, SpectrogramConfiguration>;
using NonlinearImpl = Implementation<SpectrogramSet, SpectrogramConfiguration>;

template <>
void Algorithm<SpectrogramConfiguration>::setImplementation(const Coefficients &c)
{
    if ((c.algorithmType == c.ADAPTIVE_HANN_8) || (c.algorithmType == c.ADAPTIVE_WOLA_8)) { pimpl = std::make_unique<NonlinearImpl>(c); }
    else { pimpl = std::make_unique<FilterbankImpl>(c); }
}

Spectrogram::Spectrogram(const Coefficients &c) : Algorithm<SpectrogramConfiguration>(c) {}

int SpectrogramConfiguration::getValidFFTSize(int fftSize) { return FFTConfiguration::getValidFFTSize(fftSize); }