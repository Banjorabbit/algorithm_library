#pragma once
#include "framework/framework.h"
#include "algorithm_library/filterbank.h"
#include "fft/fft_real.h"
#include "utilities/functions.h"


// --------------------------------------------------- get filterbank configurations ----------------------------------------------------------------

struct FilterbankWOLA
{

    static Eigen::ArrayXf getAnalysisWindow(const FilterbankConfiguration::Coefficients& c)
    {
        Eigen::ArrayXf window;
        const int fftSize = FFTConfiguration::getFFTSize(c.nBands);
        switch (c.filterbankType)
        {
        default: // Hann window is default case
        case FilterbankConfiguration::Coefficients::FilterbankTypes::HANN:
            if (fftSize != 4 * c.bufferSize) { throw FilterbankConfiguration::ExceptionFilterbank(c);} // Configuration not currently supported
            window = hann(fftSize);
            break;
        case FilterbankConfiguration::Coefficients::FilterbankTypes::SQRT_HANN:
            if (fftSize != 2 * c.bufferSize) { throw FilterbankConfiguration::ExceptionFilterbank(c);} // Configuration not currently supported
            window = hann(fftSize).sqrt();
            break;
        case FilterbankConfiguration::Coefficients::FilterbankTypes::WOLA:
            if (fftSize != 4 * c.bufferSize) { throw FilterbankConfiguration::ExceptionFilterbank(c);} // Configuration not currently supported
            window = sinc(2 * fftSize, 2) * kaiser(2 * fftSize, 10);
            break;
        }
        return window;
    }

    static Eigen::ArrayXf getSynthesisWindow(const FilterbankConfiguration::Coefficients& c)
    {
        Eigen::ArrayXf window;
        const int fftSize = FFTConfiguration::getFFTSize(c.nBands);
        switch (c.filterbankType)
        {
        default: // Hann window is default case
        case FilterbankConfiguration::Coefficients::FilterbankTypes::HANN:
            if (fftSize != 4 * c.bufferSize) { throw FilterbankConfiguration::ExceptionFilterbank(c);} // Configuration not currently supported
            window = hann(fftSize);
            break;
        case FilterbankConfiguration::Coefficients::FilterbankTypes::SQRT_HANN:
            if (fftSize != 2 * c.bufferSize) { throw FilterbankConfiguration::ExceptionFilterbank(c);} // Configuration not currently supported
            window = hann(fftSize).sqrt();
            break;
        case FilterbankConfiguration::Coefficients::FilterbankTypes::WOLA:
            if (fftSize != 4 * c.bufferSize) { throw FilterbankConfiguration::ExceptionFilterbank(c);} // Configuration not currently supported
            window = kaiser(2 * fftSize, 14);
            break;
        }

        // scale synthesis window to give unit output
        Eigen::ArrayXf windowSum = Eigen::ArrayXf::Zero(c.bufferSize);
        Eigen::ArrayXf windowProd = window * getAnalysisWindow(c);
        for (auto i = 0; i < window.size() / c.bufferSize; i++)
        {
            windowSum += windowProd.segment(i * c.bufferSize, c.bufferSize);
        }
        window /= windowSum.mean(); 
        return window;
    }

    // calculate delay as the group delay at 0 Hz of the prototype window:  Group Delay(z) = Real{ FFT{window * ramp} / FFT{window} } 
    static float getDelaySamples(I::Real window)
    { 
        Eigen::ArrayXf ramp = Eigen::ArrayXf::LinSpaced(window.size(), 0, window.size()-1);
        return (window * ramp).sum() / (window.sum() + 1e-12f); 
    } 

};

// --------------------------------------------------- FilterbankAnalysis ----------------------------------------------------------------

class FilterbankAnalysisWOLA : public AlgorithmImplementation<FilterbankAnalysisConfiguration, FilterbankAnalysisWOLA>
{
public:
    FilterbankAnalysisWOLA(Coefficients c =  Coefficients()) : BaseAlgorithm{c},
        fft({FFTConfiguration::getFFTSize(c.nBands)})
    {
        fftSize = FFTConfiguration::getFFTSize(c.nBands);
        window = FilterbankWOLA::getAnalysisWindow(c);
        frameSize = window.size();
        overlap = frameSize - C.bufferSize;
        nFolds = static_cast<int>(std::ceil(static_cast<float>(frameSize) / fftSize));
        maxSize = fftSize * nFolds;
        fftBuffer.resize(maxSize);
        timeBuffer.resize(frameSize, c.nChannels);

        resetVariables();
    }

    FFTReal fft;
    DEFINE_MEMBER_ALGORITHMS(fft)

    int getFFTSize() const { return fftSize; }
    int getNBands() const { return fftSize/2 + 1;}
    int getFrameSize() const { return frameSize; }

    float getDelaySamples() const { return FilterbankWOLA::getDelaySamples(window); }

    // set window if length equals frameSize
    void setWindow(I::Real win)
    {
        if (win.size() == frameSize)
        {
            C.filterbankType = C.USER_DEFINED; // force overwrite
            window = win;
        }
    }

private:
    
    inline void processOn(Input xTime, Output yFreq)
    {
        timeBuffer.topRows(overlap) = timeBuffer.bottomRows(overlap);
        timeBuffer.bottomRows(C.bufferSize) = xTime;
        for (auto channel = 0; channel < C.nChannels; channel++)
        {
            fftBuffer.head(frameSize) = timeBuffer.col(channel) * window;
            for (auto j = 1; j < nFolds; j++)
            {
                fftBuffer.head(fftSize) += fftBuffer.segment(j * fftSize, fftSize);
            }
            fft.process(fftBuffer.head(fftSize), yFreq.col(channel));
        }
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

    int frameSize, fftSize;
    Eigen::ArrayXf window, fftBuffer; 
    Eigen::ArrayXXf timeBuffer;
    int overlap, nFolds, maxSize;

    friend BaseAlgorithm;
};


// --------------------------------------------------- FilterbankSynthesis ----------------------------------------------------------------

class FilterbankSynthesisWOLA : public AlgorithmImplementation<FilterbankSynthesisConfiguration, FilterbankSynthesisWOLA>
{
public:
    FilterbankSynthesisWOLA(Coefficients c = Coefficients()) : BaseAlgorithm{c},
        fft({ FFTConfiguration::getFFTSize(c.nBands) })
    {
        fftSize = FFTConfiguration::getFFTSize(c.nBands);
        window = FilterbankWOLA::getSynthesisWindow(c);
        frameSize = window.size();
        overlap = frameSize - C.bufferSize;
        nFolds = static_cast<int>(std::ceil(static_cast<float>(frameSize) / fftSize));
        maxSize = fftSize * nFolds;
        window.resize(frameSize);
        fftBuffer.resize(maxSize);
        timeBuffer.resize(frameSize, C.nChannels);

        resetVariables();
    }

    FFTReal fft;
    DEFINE_MEMBER_ALGORITHMS(fft)

    int getFFTSize() const { return fftSize; }
    int getNBands() const { return fftSize/2 + 1;}

    float getDelaySamples() const { return FilterbankWOLA::getDelaySamples(window); }

private:

    inline void processOn(Input xFreq, Output yTime)
    {
        for (auto channel = 0; channel < C.nChannels; channel++)
        {
            fft.inverse(xFreq.col(channel), fftBuffer.head(fftSize));
            for (auto j = 1; j < nFolds; j++)
            {
                fftBuffer.segment(j * fftSize, fftSize) = fftBuffer.head(fftSize);
            }
            timeBuffer.col(channel) += (fftBuffer.tail(frameSize) * window);
        }
        yTime = timeBuffer.topRows(C.bufferSize);
        timeBuffer.topRows(overlap) = timeBuffer.bottomRows(overlap);
        timeBuffer.bottomRows(C.bufferSize) = 0.f;
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

    int fftSize, frameSize;
    Eigen::ArrayXf window, fftBuffer; 
    Eigen::ArrayXXf timeBuffer;
    int overlap, nFolds, maxSize;

    friend BaseAlgorithm;
};
