#include "spectrogram/spectrogram_filterbank.h"
#include "spectrogram/spectrogram_nonlinear.h"

using FilterbankImpl = Impl<SpectrogramFilterbank, SpectrogramConfiguration>;
using NonlinearImpl = Impl<SpectrogramNonlinear, SpectrogramConfiguration>;

template<> 
Algorithm<SpectrogramConfiguration>::Algorithm() 
{
    if (SpectrogramConfiguration::Coefficients().spectrogramType == SpectrogramConfiguration::Coefficients::FILTERBANK)
    {
        pimpl = std::make_unique<FilterbankImpl>();
    }
    else
    {
        pimpl = std::make_unique<NonlinearImpl>();
    }
} 

template<> 
Algorithm<SpectrogramConfiguration>::~Algorithm() {} 

template<>
Algorithm<SpectrogramConfiguration>::Algorithm(const Coefficients& c) 
{
    if (c.spectrogramType == c.FILTERBANK)
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
    if (c.spectrogramType == c.FILTERBANK)
    {
        static_cast<FilterbankImpl*>(pimpl.get())->algo.setWindow(window);
    }
    else
    {
        static_cast<NonlinearImpl*>(pimpl.get())->algo.setWindow(window);
    }
}

int Spectrogram::getNFrames(int inputSize, int bufferSize) 
{
    return SpectrogramFilterbank::getNFrames(inputSize, bufferSize);
}

int Spectrogram::getValidFFTSize(int fftSize)
{
    return FFTReal::getValidFFTSize(fftSize);
}