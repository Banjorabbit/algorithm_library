#include "fft/fft_real.h"

DEFINE_CONSTRUCTOR_DESTRUCTOR(FFT, FFTReal, FFTConfiguration)

void FFT::inverse(I::Complex2D xFreq, O::Real2D yTime)
{
	static_cast<FFTRealSingleBufferImpl*>(pimpl.get())->algo.inverse(xFreq, yTime);
}

bool FFT::isFFTSizeValid(const int fftSize)
{
	return FFTReal::isFFTSizeValid(fftSize);
}

const std::array<int,72> FFTReal::validFFTSizes  = {32, 64, 96, 128, 160, 192, 256, 288, 320, 384, 480, 512, 576, 640, 768, 800, 864, 960, 1024, 1152, 1280, 1440, 1536, 1600, 1728, 1920, 2048, 2304, 2400, 2560, 2592, 2880, 3072, 3200, 3456, 3840, 4000, 4320, 4608, 4800, 5120, 5184, 5760, 6144, 6400, 6912, 7200, 7680, 7776, 8000, 8640, 9216, 9600, 10240, 10368, 11520, 12000, 12800, 12960, 13824, 14400, 15360, 15552, 16000, 17280, 18432, 19200, 20000, 20736, 21600, 23040, 23328};