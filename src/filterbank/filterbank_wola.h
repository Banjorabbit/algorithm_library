#pragma once
#include "framework/framework.h"
#include "algorithm_library/filterbank.h"
#include "fft/fft_real.h"
#include "utilities/functions.h"

// --------------------------------------------------- FilterbankAnalysis ----------------------------------------------------------------

class FilterbankAnalysisWOLA : public AlgorithmImplementation<FilterbankAnalysisConfiguration, FilterbankAnalysisWOLA>
{
public:
    FilterbankAnalysisWOLA(Coefficients c =  Coefficients()) :
        AlgorithmImplementation<FilterbankAnalysisConfiguration, FilterbankAnalysisWOLA>{ c },
        fft({ C.fftSize })
    {
        initialize();
    }

    FFTReal fft;
    DEFINE_MEMBER_ALGORITHMS(fft)

    inline void processOn(Input xTime, Output yFreq)
    {
        timeBuffer.topRows(overlap) = timeBuffer.bottomRows(overlap);
        timeBuffer.bottomRows(C.bufferSize) = xTime;
        for (auto channel = 0; channel < C.nChannels; channel++)
        {
            fftBuffer.head(C.frameSize) = timeBuffer.col(channel) * window;
            for (auto j = 1; j < nFolds; j++)
            {
                fftBuffer.head(C.fftSize) += fftBuffer.segment(j * C.fftSize, C.fftSize);
            }
            fft.process(fftBuffer.head(C.fftSize), yFreq.col(channel));
        }
    }

    void onParametersChanged()
    {
        switch (P.windowType)
        {
        case Parameters::HANN_WINDOW:
            window = hann(C.frameSize) * C.gain;
            break;
        case Parameters::SQRT_HANN_WINDOW:
            window = hann(C.frameSize).sqrt() * C.gain;
            break;
        case Parameters::RECTANGULAR:
            window.setOnes();
            window *= C.gain;
            break;
        case Parameters::USER_DEFINED:
            break;
        default:
            window.setZero();
        }
    }

    void setWindow(I::Real w)
    {
        if (P.windowType == P.USER_DEFINED)
        {
            window = w;
        }
    }

    Eigen::ArrayXf getWindow() const
    {
        return window;
    }

    void setStandardFilterbank(int bufferSize)
    {
        const int nChannels = getCoefficients().nChannels;
        setStandardFilterbank(bufferSize, nChannels);
    }

    void setStandardFilterbank(int bufferSize, int nChannels)
    {
        auto s = getSetup();
        s.coefficients.bufferSize = bufferSize;
        s.coefficients.fftSize = 4 * bufferSize;
        s.coefficients.frameSize = 4 * bufferSize;
        s.coefficients.gain = 1;
        s.coefficients.nChannels = nChannels;
        s.parameters.windowType = s.parameters.USER_DEFINED;
        setSetup(s);
        setWindow(sinc(s.coefficients.frameSize, 1) * kaiser(s.coefficients.frameSize, 4));
    }

    void setLowDelayFilterbank(int bufferSize)
    {
        const int nChannels = getCoefficients().nChannels;
        setLowDelayFilterbank(bufferSize, nChannels);
    }

    void setLowDelayFilterbank(int bufferSize, int nChannels)
    {
        auto s = getSetup();
        s.coefficients.bufferSize = bufferSize;
        s.coefficients.fftSize = 16 * bufferSize;
        s.coefficients.frameSize = 16 * bufferSize;
        s.coefficients.gain = 1;
        s.coefficients.nChannels = nChannels;
        s.parameters.windowType = s.parameters.USER_DEFINED;
        setSetup(s);
        Eigen::ArrayXf win1 = hann(4 * bufferSize);
        Eigen::ArrayXf win2 = hann(28 * bufferSize);
        Eigen::ArrayXf w(s.coefficients.frameSize);
        w << win2.head(14 * bufferSize), win1.tail(2 * bufferSize);
        setWindow(w);
    }

    void setHighQualityFilterbank(int bufferSize)
    {
        const int nChannels = getCoefficients().nChannels;
        setHighQualityFilterbank(bufferSize, nChannels);
    }

    void setHighQualityFilterbank(int bufferSize, int nChannels)
    {
        auto s = getSetup();
        s.coefficients.bufferSize = bufferSize;
        s.coefficients.fftSize = 4 * bufferSize;
        s.coefficients.frameSize = 8 * bufferSize;
        s.coefficients.gain = 1;
        s.coefficients.nChannels = nChannels;
        s.parameters.windowType = s.parameters.USER_DEFINED;
        setSetup(s);
        setWindow(sinc(s.coefficients.frameSize, 2) * kaiser(s.coefficients.frameSize, 10));
    }
    
    // calculate delay as the group delay at 0 Hz of the prototype window ( D(z) = Real{ FFT{window * ramp} / FFT{window} } )
    int getDelaySamples() const 
    { 
        Eigen::ArrayXf ramp = Eigen::ArrayXf::LinSpaced(window.size(), 0, window.size()-1);
        return (ramp * window).sum() / window.sum(); 
    } 

private:
    
    void initialize()
    {
        overlap = C.frameSize - C.bufferSize;
        nFolds = static_cast<int>(std::ceil(static_cast<float>(C.frameSize) / C.fftSize));
        maxSize = C.fftSize * nFolds;
        window.resize(C.frameSize);
        fftBuffer.resize(maxSize);
        timeBuffer.resize(C.frameSize, C.nChannels);

        onParametersChanged();

        resetVariables();
    }

    size_t getDynamicSizeVariables() const final
    {
        auto size = window.getDynamicMemorySize();
        size += fftBuffer.getDynamicMemorySize();
        size += timeBuffer.getDynamicMemorySize();
        return size;
    }

    void resetVariables() final 
    {
        fftBuffer.setZero();
        timeBuffer.setZero();
    }

    Eigen::ArrayXf window, fftBuffer; // advantage of FFTBuffer being allocated on heap is that zeropadding is kept between calls
    Eigen::ArrayXXf timeBuffer;
    int overlap, nFolds, maxSize;
};


// --------------------------------------------------- FilterbankSynthesis ----------------------------------------------------------------

class FilterbankSynthesisWOLA : public AlgorithmImplementation<FilterbankSynthesisConfiguration, FilterbankSynthesisWOLA>
{
public:
    FilterbankSynthesisWOLA(Coefficients c = Coefficients()) :
        AlgorithmImplementation<FilterbankSynthesisConfiguration, FilterbankSynthesisWOLA>{ c },
        fft({ C.fftSize })
    {
        initialize();
    }

    FFTReal fft;
    DEFINE_MEMBER_ALGORITHMS(fft)

    inline void processOn(Input xFreq, Output yTime)
    {
        for (auto channel = 0; channel < C.nChannels; channel++)
        {
            fft.inverse(xFreq.col(channel), fftBuffer.head(C.fftSize));
            for (auto j = 1; j < nFolds; j++)
            {
                fftBuffer.segment(j * C.fftSize, C.fftSize) = fftBuffer.head(C.fftSize);
            }
            timeBuffer.col(channel) += (fftBuffer.tail(C.frameSize) * window);
        }
        yTime = timeBuffer.topRows(C.bufferSize);
        timeBuffer.topRows(overlap) = timeBuffer.bottomRows(overlap);
        timeBuffer.bottomRows(C.bufferSize) = 0.f;
    }

    void setWindow(I::Real w)
    {
        if (P.windowType == P.USER_DEFINED)
        {
            window = w;
        }
    }

    void onParametersChanged()
    {
        switch (P.windowType)
        {
        case Parameters::HANN_WINDOW:
            window = hann(C.frameSize) * C.gain;
            break;
        case Parameters::SQRT_HANN_WINDOW:
            window = hann(C.frameSize).sqrt() * C.gain;
            break;
        case Parameters::RECTANGULAR:
            window.setOnes();
            window *= C.gain;
            break;
        case Parameters::USER_DEFINED:
            break;
        default:
            window.setZero();
        }
    }

    void setStandardFilterbank(int bufferSize)
    {
        const int nChannels = getCoefficients().nChannels;
        setStandardFilterbank(bufferSize, nChannels);
    }

    void setStandardFilterbank(int bufferSize, int nChannels)
    {
        auto s = getSetup();
        s.coefficients.bufferSize = bufferSize;
        s.coefficients.fftSize = 4 * bufferSize;
        s.coefficients.frameSize = 4 * bufferSize;
        s.coefficients.gain = 1;
        s.coefficients.nChannels = nChannels;
        s.parameters.windowType = s.parameters.USER_DEFINED;
        setSetup(s);
        Eigen::ArrayXf w = sinc(s.coefficients.frameSize, 1) * kaiser(s.coefficients.frameSize, 4);
        Eigen::ArrayXf windowSum = Eigen::ArrayXf::Zero(bufferSize);
        for (auto i = 0; i < 4; i++)
        {
            windowSum += w.segment(i * bufferSize, bufferSize).abs2();
        }
        w /= windowSum.mean(); // analysis window is scaled to 1 so only synthesis window is scaled with mean
        setWindow(w);
    }

    void setLowDelayFilterbank(int bufferSize)
    {
        const int nChannels = getCoefficients().nChannels;
        setLowDelayFilterbank(bufferSize, nChannels);
    }

    void setLowDelayFilterbank(int bufferSize, int nChannels)
    {
        auto s = getSetup();
        s.coefficients.bufferSize = bufferSize;
        s.coefficients.fftSize = 16 * bufferSize;
        s.coefficients.frameSize = 4 * bufferSize;
        s.coefficients.gain = 1;
        s.coefficients.nChannels = nChannels;
        s.parameters.windowType = s.parameters.USER_DEFINED;
        setSetup(s);
        Eigen::ArrayXf w = hann(s.coefficients.frameSize) * 0.573576926471298f;
        setWindow(w);
    }

    void setHighQualityFilterbank(int bufferSize)
    {
        const int nChannels = getCoefficients().nChannels;
        setHighQualityFilterbank(bufferSize, nChannels);
    }

    void setHighQualityFilterbank(int bufferSize, int nChannels)
    {
        auto s = getSetup();
        s.coefficients.bufferSize = bufferSize;
        s.coefficients.fftSize = 4 * bufferSize;
        s.coefficients.frameSize = 8 * bufferSize;
        s.coefficients.gain = 1;
        s.coefficients.nChannels = nChannels;
        s.parameters.windowType = s.parameters.USER_DEFINED;
        setSetup(s);
        Eigen::ArrayXf w = kaiser(s.coefficients.frameSize, 14) * 0.619944139445609f;
        setWindow(w);
    }

        // calculate delay as the group delay at 0 Hz of the prototype window ( D(z) = Real{ FFT{window * ramp} / FFT{window} } )
    int getDelaySamples() const 
    { 
        Eigen::ArrayXf ramp = Eigen::ArrayXf::LinSpaced(window.size(), 0, window.size()-1);
        return (ramp * window).sum() / window.sum(); 
    } 
private:

    void initialize()
    {
        overlap = C.frameSize - C.bufferSize;
        nFolds = static_cast<int>(std::ceil(static_cast<float>(C.frameSize) / C.fftSize));
        maxSize = C.fftSize * nFolds;
        window.resize(C.frameSize);
        fftBuffer.resize(maxSize);
        timeBuffer.resize(C.frameSize, C.nChannels);

        onParametersChanged();

        resetVariables();
    }

    size_t getDynamicSizeVariables() const final
    {
        auto size = window.getDynamicMemorySize();
        size += fftBuffer.getDynamicMemorySize();
        size += timeBuffer.getDynamicMemorySize();
        return size;
    }

    void resetVariables() final
    {
        fftBuffer.setZero();
        timeBuffer.setZero();
    }

    Eigen::ArrayXf window, fftBuffer; // advantage of FFTBuffer being allocated on heap is that zeropadding is kept between calls
    Eigen::ArrayXXf timeBuffer;
    int overlap, nFolds, maxSize;
};
