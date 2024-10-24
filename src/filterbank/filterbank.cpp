#include "filterbank/filterbank_single_channel.h"
#include "filterbank/filterbank_wola.h"

template <>
void Algorithm<FilterbankAnalysisConfiguration>::setImplementation(const Coefficients &c)
{
    if (c.nChannels == 1) { pimpl = std::make_unique<Implementation<FilterbankAnalysisSingleChannel, FilterbankAnalysisConfiguration>>(c); }
    else { pimpl = std::make_unique<Implementation<FilterbankAnalysisWOLA, FilterbankAnalysisConfiguration>>(c); }
}

FilterbankAnalysis::FilterbankAnalysis(const Coefficients &c) : Algorithm<FilterbankAnalysisConfiguration>(c) {}

template <>
void Algorithm<FilterbankSynthesisConfiguration>::setImplementation(const Coefficients &c)
{
    if (c.nChannels == 1) { pimpl = std::make_unique<Implementation<FilterbankSynthesisSingleChannel, FilterbankSynthesisConfiguration>>(c); }
    else { pimpl = std::make_unique<Implementation<FilterbankSynthesisWOLA, FilterbankSynthesisConfiguration>>(c); }
}

FilterbankSynthesis::FilterbankSynthesis(const Coefficients &c) : Algorithm<FilterbankSynthesisConfiguration>(c) {}

float FilterbankAnalysis::getDelaySamples() const
{
    if (getCoefficients().nChannels == 1)
    {
        return static_cast<Implementation<FilterbankAnalysisSingleChannel, FilterbankAnalysisConfiguration> *>(pimpl.get())->algo.getDelaySamples();
    }
    else { return static_cast<Implementation<FilterbankAnalysisWOLA, FilterbankAnalysisConfiguration> *>(pimpl.get())->algo.getDelaySamples(); }
}

float FilterbankSynthesis::getDelaySamples() const
{
    if (getCoefficients().nChannels == 1)
    {
        return static_cast<Implementation<FilterbankSynthesisSingleChannel, FilterbankSynthesisConfiguration> *>(pimpl.get())->algo.getDelaySamples();
    }
    else { return static_cast<Implementation<FilterbankSynthesisWOLA, FilterbankSynthesisConfiguration> *>(pimpl.get())->algo.getDelaySamples(); }
}

namespace FilterbankShared
{

bool isCoefficientsValid(const FilterbankConfiguration::Coefficients &c)
{
    const int fftSize = FFTConfiguration::convertNBandsToFFTSize(c.nBands);
    const float factor = static_cast<float>(fftSize) / c.bufferSize;
    const float factorFloor = std::floor(factor);
    switch (c.filterbankType)
    {
    default: // Hann window is default case
    case FilterbankConfiguration::Coefficients::FilterbankTypes::HANN:
        if ((factor < 2.f) || (factor != factorFloor)) { return false; } // Configuration not currently supported
        break;
    case FilterbankConfiguration::Coefficients::FilterbankTypes::SQRT_HANN:
        if (factor != 2.f) { return false; } // Configuration not currently supported
        break;
    case FilterbankConfiguration::Coefficients::FilterbankTypes::WOLA:
        if (factor != 4.f) { return false; } // Configuration not currently supported
        break;
    }
    return true;
}

Eigen::ArrayXf getAnalysisWindow(const FilterbankConfiguration::Coefficients &c)
{
    Eigen::ArrayXf window;
    const int fftSize = FFTConfiguration::convertNBandsToFFTSize(c.nBands);
    switch (c.filterbankType)
    {
    default: // Hann window is default case
    case FilterbankConfiguration::Coefficients::FilterbankTypes::HANN: window = hann(fftSize); break;
    case FilterbankConfiguration::Coefficients::FilterbankTypes::SQRT_HANN: window = hann(fftSize).sqrt(); break;
    case FilterbankConfiguration::Coefficients::FilterbankTypes::WOLA: window = sinc(2 * fftSize, 2) * kaiser(2 * fftSize, 10); break;
    }
    return window;
}

Eigen::ArrayXf getSynthesisWindow(const FilterbankConfiguration::Coefficients &c)
{
    Eigen::ArrayXf window;
    const int fftSize = FFTConfiguration::convertNBandsToFFTSize(c.nBands);
    switch (c.filterbankType)
    {
    default: // Hann window is default case
    case FilterbankConfiguration::Coefficients::FilterbankTypes::HANN:
        if ((fftSize / c.bufferSize) == 2) { window = Eigen::ArrayXf::Ones(fftSize); }
        else { window = hann(fftSize); }
        break;
    case FilterbankConfiguration::Coefficients::FilterbankTypes::SQRT_HANN: window = hann(fftSize).sqrt(); break;
    case FilterbankConfiguration::Coefficients::FilterbankTypes::WOLA: window = kaiser(2 * fftSize, 14); break;
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
    Eigen::ArrayXf ramp = Eigen::ArrayXf::LinSpaced(window.size(), 0, static_cast<float>(window.size() - 1));
    return (window * ramp).sum() / (window.sum() + 1e-12f);
}
}; // namespace FilterbankShared