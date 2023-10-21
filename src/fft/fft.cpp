#include "fft/fft_real.h"

DEFINE_CONSTRUCTOR_DESTRUCTOR(FFT, FFTReal, FFTConfiguration)

void FFT::inverse(I::Complex2D xFreq, O::Real2D yTime)
{
	static_cast<FFTRealImpl*>(pimpl.get())->algo.inverse(xFreq, yTime);
}

bool FFT::isFFTSizeValid(const int fftSize)
{
	return FFTReal::isFFTSizeValid(fftSize);
}
