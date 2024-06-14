#pragma once
#include "algorithm_library/fft.h"
#include "framework/framework.h"
#include "utilities/pffft.h"

// Wrapper for real pffft.
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
    FFTReal(Coefficients c = Coefficients())
        : BaseAlgorithm{c}, scale{1.f / static_cast<float>(C.fftSize)}, setup{std::shared_ptr<PFFFT_Setup>(pffftSmartCreate(C.fftSize), pffftSmartDestroy)}
    {
        out.resize((int)C.fftSize);
        if (!setup) { throw Configuration::ExceptionFFT(C.fftSize); }
        assert(Configuration::isFFTSizeValid(
            C.fftSize)); // assert that FFT size is valid. It is not extremely useful after the throwing exception, but it allows the exception to be caught in debug mode
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

  private:
    inline void processAlgorithm(Input xTime, Output yFreq)
    {
        // After the first channel, yFreq is not 16 byte alligned due to FFTSize/2+1 size so we can't write output to it from FFT transform
        for (auto channel = 0; channel < xTime.cols(); channel++)
        {
            pffft_transform_ordered(setup.get(), xTime.col(channel).data(), out.data(), nullptr, PFFFT_FORWARD);
            yFreq(0, channel) = out(0);
            yFreq(C.fftSize / 2, channel) = out(1);
            std::memcpy(&yFreq.real()(1, channel), &out(2), (C.fftSize - 2) * sizeof(float));
        }
    }

    bool isCoefficientsValid() const final
    {
        bool flag = Configuration::isFFTSizeValid(C.fftSize);
        return flag;
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = out.getDynamicMemorySize();
        if (setup) { size += pffft_get_setup_size(setup.get()); }
        return size;
    }

    // defined in fft.cpp
    static void pffftSmartDestroy(PFFFT_Setup *s);
    static PFFFT_Setup *pffftSmartCreate(int fftSize);

    float scale;
    std::shared_ptr<PFFFT_Setup> setup;
    Eigen::ArrayXf out;

    friend BaseAlgorithm;
};
