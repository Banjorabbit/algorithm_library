#pragma once
#include "framework/framework.h"
#include "utilities/pffft.h"
#include "algorithm_library/fft.h"

// Wrapper for real pffft. 
//
// pffft requires 16 byte alligned data, and we don't know the allocation of input/output (float is 4 bytes). 
// The assert in pffft.c is: #define VALIGNED(ptr) ((((uintptr_t)(ptr)) & 0xF) == 0)
// Especially, frequency domain data has length FFTSize/2+1 so if first channel is 16 byte alligned then second channel is guaranteed not to be 16 byte alligned.
// For this reason, FFT transform is using memory allocated on the stack.
//
// This class has a public inverse FFT function.
// Supported FFT sizes are multiples of 32 and FFT class has a public isFFTSizeValid() method.
// 
// NOTE: This class will throw an exception if FFT size is not supported. 
//
// author: Kristian Timm Andersen

class FFTReal : public AlgorithmImplementation<FFTConfiguration, FFTReal>
{
public:
    FFTReal(Coefficients c = Coefficients()) :
        AlgorithmImplementation<FFTConfiguration, FFTReal>{ c },
        scale{ 1.f / static_cast<float>(C.fftSize) },
        setup{ std::shared_ptr<PFFFT_Setup>(pffftSmartCreate(C.fftSize), pffftSmartDestroy) }
    {
        if (!setup) { throw Configuration::ExceptionFFT(C.fftSize); }
        assert(isFFTSizeValid(C.fftSize)); // assert that FFT size is valid. It is not extremely useful after the throwing exception, but it allows the exception to be caught in debug mode
    }

    inline void processOn(Input xTime, Output yFreq)
    {
        // After the first channel, yFreq is not 16 byte alligned due to FFTSize/2+1 size so we can't write output to it from FFT transform
        for (auto channel = 0; channel < xTime.cols(); channel++)
        {
            Eigen::ArrayXf out((int)C.fftSize);
            pffft_transform_ordered(setup.get(), xTime.col(channel).data(), out.data(), nullptr, PFFFT_FORWARD);
            yFreq(0, channel) = out(0);
            yFreq(C.fftSize / 2, channel) = out(1);
            std::memcpy(&yFreq.real()(1, channel), &out(2), (C.fftSize - 2) * sizeof(float));
        }
    }

    inline void inverse(I::Complex2D xFreq, O::Real2D yTime)
    {
        for (auto channel = 0; channel < xFreq.cols(); channel++)
        {
            yTime(0, channel) = xFreq(0, channel).real();
            yTime(1, channel) = xFreq(C.fftSize / 2, channel).real();
            std::memcpy(&yTime(2, channel), xFreq.col(channel).data() + 1, (C.fftSize - 2) * sizeof(float));
            pffft_transform_ordered(setup.get(), yTime.col(channel).data(), yTime.col(channel).data(), nullptr, PFFFT_BACKWARD);
            yTime.col(channel) *= scale;
        }
    }

    static inline bool isFFTSizeValid(const int fftSize)
    {
        if (fftSize % 32 != 0 || fftSize < 32) { return false; } // first check size is integer factor of 32
        PFFFT_Setup *setup = pffft_new_setup(fftSize, PFFFT_REAL);
        if (!setup) { return false; }
        pffft_destroy_setup(setup);
        return true;
    }

    static int getValidFFTSize(int fftSize) 
    { 
        if (fftSize > validFFTSizes.back())
        {
            return static_cast<int>(std::pow(2, std::ceil(std::log2(fftSize)))); // return power of 2
        }
        return *std::upper_bound(validFFTSizes.begin(), validFFTSizes.end(), fftSize);
    }

private:

    size_t getMembersDynamicSize() const final
    {
        if (setup) { return pffft_get_setup_size(setup.get()); }
        return 0;
    }

    float scale;
    std::shared_ptr<PFFFT_Setup> setup;
    
    static void pffftSmartDestroy(PFFFT_Setup* s) { if (s != nullptr) { pffft_destroy_setup(s); } } // only call delete function if shared pointer is not nullptr
    static PFFFT_Setup* pffftSmartCreate(int fftSize) 
    { 
        if (fftSize % 32 == 0 && fftSize >= 32)  { return pffft_new_setup(fftSize, PFFFT_REAL); } 
        return nullptr;
    } 

    static const std::array<int,72> validFFTSizes; // defined in fft.cpp
};

