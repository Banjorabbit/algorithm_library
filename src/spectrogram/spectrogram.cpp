#include "spectrogram/spectrogram_filterbank.h"
#include "spectrogram/spectrogram_nonlinear.h"

using FilterbankImpl = Implementation<SpectrogramFilterbank, SpectrogramConfiguration>;
using NonlinearImpl = Implementation<SpectrogramNonlinear, SpectrogramConfiguration>;

template<>
Algorithm<SpectrogramConfiguration>::Algorithm(const Coefficients& c) 
{
    if (c.algorithmType == c.NONLINEAR)
    {
        pimpl = std::make_unique<NonlinearImpl>(c);
    }
    else
    {
        pimpl = std::make_unique<FilterbankImpl>(c);
    }
} 

Spectrogram::Spectrogram(const Coefficients& c) : Algorithm<SpectrogramConfiguration>(c) {}

int Spectrogram::getValidFFTSize(int fftSize)
{
    return FFTReal::getValidFFTSize(fftSize);
}