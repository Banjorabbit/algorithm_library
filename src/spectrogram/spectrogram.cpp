#include "spectrogram/spectrogram_filterbank.h"

DEFINE_SOURCE_INTERFACE(SpectrogramConfiguration, SpectrogramFilterbank)

Spectrogram::Spectrogram(const Coefficients& c) : Algorithm<SpectrogramConfiguration>(c) {}

void Spectrogram::setWindow(I::Real window)
{
    pimpl->algo.setWindow(window);
}

int Spectrogram::getNFrames(int inputSize, int bufferSize) 
{
    return SpectrogramFilterbank::getNFrames(inputSize, bufferSize);
}