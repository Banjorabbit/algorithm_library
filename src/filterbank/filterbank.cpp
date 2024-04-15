#include "filterbank/filterbank_wola.h"

DEFINE_ALGORITHM_CONSTRUCTOR(FilterbankAnalysis, FilterbankAnalysisWOLA, FilterbankAnalysisConfiguration)
DEFINE_ALGORITHM_CONSTRUCTOR(FilterbankSynthesis, FilterbankSynthesisWOLA, FilterbankSynthesisConfiguration)

float FilterbankAnalysis::getDelaySamples() const
{
    return static_cast<FilterbankAnalysisWOLASingleBufferImpl*>(pimpl.get())->algo.getDelaySamples();
}

float FilterbankSynthesis::getDelaySamples() const
{
    return static_cast<FilterbankSynthesisWOLASingleBufferImpl*>(pimpl.get())->algo.getDelaySamples();
}

namespace FilterbankWOLA
{

    Eigen::ArrayXf getAnalysisWindow(const FilterbankConfiguration::Coefficients& c)
    {
        Eigen::ArrayXf window;
        const int fftSize = FFTConfiguration::convertNBandsToFFTSize(c.nBands);
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

    Eigen::ArrayXf getSynthesisWindow(const FilterbankConfiguration::Coefficients& c)
    {
        Eigen::ArrayXf window;
        const int fftSize = FFTConfiguration::convertNBandsToFFTSize(c.nBands);
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
    float getDelaySamples(I::Real window)
    { 
        Eigen::ArrayXf ramp = Eigen::ArrayXf::LinSpaced(window.size(), 0, window.size()-1);
        return (window * ramp).sum() / (window.sum() + 1e-12f); 
    } 
};