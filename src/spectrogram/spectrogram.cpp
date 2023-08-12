#include "spectrogram/spectrogram_filterbank.h"
#include "spectrogram/spectrogram_nonlinear.h"

// resolve type of spectrogram algorithm in SpectrogramConfiguration
using SpectrogramType = std::conditional_t<SpectrogramConfiguration::spectrogramType == SpectrogramConfiguration::FILTERBANK, SpectrogramFilterbank, SpectrogramNonlinear>;

DEFINE_SOURCE_INTERFACE(SpectrogramConfiguration, SpectrogramType)

Spectrogram::Spectrogram(const Coefficients& c) : Algorithm<SpectrogramConfiguration>(c) {}

void Spectrogram::setWindow(I::Real window)
{
    pimpl->algo.setWindow(window);
}

int Spectrogram::getNFrames(int inputSize, int bufferSize) 
{
    return SpectrogramType::getNFrames(inputSize, bufferSize);
}

int Spectrogram::getValidFFTSize(int fftSize)
{
    return FFTReal::getValidFFTSize(fftSize);
}