#include "fft/fft_real.h"

DEFINE_SOURCE_INTERFACE(FFTConfiguration, FFTReal)

FFT::FFT(const Coefficients& c) : Algorithm<FFTConfiguration>(c) {}

void FFT::inverse(I::Complex2D xFreq, O::Real2D yTime)
{
	pimpl->algo.inverse(xFreq, yTime);
}

bool FFT::isFFTSizeValid(const int fftSize)
{
	return FFTReal::isFFTSizeValid(fftSize);
}