#include "fft/fft_real.h"

template<> 
Algorithm<FFTConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<FFTReal, FFTConfiguration>>();
} 

template<> 
Algorithm<FFTConfiguration>::~Algorithm() {} 

template<>
Algorithm<FFTConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<FFTReal, FFTConfiguration>>(c);
} 

FFT::FFT(const Coefficients& c) : Algorithm<FFTConfiguration>(c) {}

void FFT::inverse(I::Complex2D xFreq, O::Real2D yTime)
{
	static_cast<Impl<FFTReal, FFTConfiguration>*>(pimpl.get())->algo.inverse(xFreq, yTime);
}

bool FFT::isFFTSizeValid(const int fftSize)
{
	return FFTReal::isFFTSizeValid(fftSize);
}