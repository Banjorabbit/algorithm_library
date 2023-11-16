#include "spectrogram/spectrogram_filterbank.h"
#include "spectrogram/spectrogram_nonlinear.h"

using FilterbankImpl = Implementation<SpectrogramFilterbank, SpectrogramConfiguration>;
using NonlinearImpl = Implementation<SpectrogramNonlinear, SpectrogramConfiguration>;

template<>
Algorithm<SpectrogramConfiguration>::Algorithm(const Coefficients& c) 
{
    if (c.algorithm == c.FILTERBANK)
    {
        pimpl = std::make_unique<FilterbankImpl>(c);
    }
    else
    {
        pimpl = std::make_unique<NonlinearImpl>(c);
    }
} 

Spectrogram::Spectrogram(const Coefficients& c) : Algorithm<SpectrogramConfiguration>(c) {}

void Spectrogram::setWindow(I::Real window)
{
    const auto c = getCoefficients();
    if (c.algorithm == c.FILTERBANK)
    {
        static_cast<FilterbankImpl*>(pimpl.get())->algo.setWindow(window);
    }
    else
    {
        static_cast<NonlinearImpl*>(pimpl.get())->algo.setWindow(window);
    }
}

// static function so it doesn't matter which getNFrames we are using
int Spectrogram::getNFrames(int inputSize, int bufferSize) 
{
    return SpectrogramFilterbank::getNFrames(inputSize, bufferSize);
}

int Spectrogram::getValidFFTSize(int fftSize)
{
    return FFTReal::getValidFFTSize(fftSize);
}