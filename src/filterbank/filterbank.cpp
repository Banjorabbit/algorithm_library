#include "filterbank/filterbank_wola.h"

DEFINE_SOURCE_INTERFACE(FilterbankAnalysisConfiguration, FilterbankAnalysisWOLA)

FilterbankAnalysis::FilterbankAnalysis(const Coefficients& c) : Algorithm<FilterbankAnalysisConfiguration>{ c } {}
void FilterbankAnalysis::setWindow(I::Real window) { pimpl->algo.setWindow(window); }
void FilterbankAnalysis::setStandardFilterbank(int bufferSize, int nChannels) { pimpl->algo.setStandardFilterbank(bufferSize, nChannels); }
void FilterbankAnalysis::setLowDelayFilterbank(int bufferSize, int nChannels) { pimpl->algo.setLowDelayFilterbank(bufferSize, nChannels); }
void FilterbankAnalysis::setHighQualityFilterbank(int bufferSize, int nChannels) { pimpl->algo.setHighQualityFilterbank(bufferSize, nChannels); }

DEFINE_SOURCE_INTERFACE(FilterbankSynthesisConfiguration, FilterbankSynthesisWOLA)

FilterbankSynthesis::FilterbankSynthesis(const Coefficients& c) : Algorithm<FilterbankSynthesisConfiguration>{ c } {}
void FilterbankSynthesis::setWindow(I::Real window) { pimpl->algo.setWindow(window); }
void FilterbankSynthesis::setStandardFilterbank(int bufferSize, int nChannels) { pimpl->algo.setStandardFilterbank(bufferSize, nChannels); }
void FilterbankSynthesis::setLowDelayFilterbank(int bufferSize, int nChannels) { pimpl->algo.setLowDelayFilterbank(bufferSize, nChannels); }
void FilterbankSynthesis::setHighQualityFilterbank(int bufferSize, int nChannels) { pimpl->algo.setHighQualityFilterbank(bufferSize, nChannels); }