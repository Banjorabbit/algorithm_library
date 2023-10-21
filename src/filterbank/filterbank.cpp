#include "filterbank/filterbank_wola.h"

DEFINE_CONSTRUCTOR_DESTRUCTOR(FilterbankAnalysis, FilterbankAnalysisWOLA, FilterbankAnalysisConfiguration)
DEFINE_CONSTRUCTOR_DESTRUCTOR(FilterbankSynthesis, FilterbankSynthesisWOLA, FilterbankSynthesisConfiguration)

void FilterbankAnalysis::setWindow(I::Real window) { static_cast<FilterbankAnalysisWOLAImpl*>(pimpl.get())->algo.setWindow(window); }
void FilterbankAnalysis::setStandardFilterbank(int bufferSize, int nChannels) { static_cast<FilterbankAnalysisWOLAImpl*>(pimpl.get())->algo.setStandardFilterbank(bufferSize, nChannels); }
void FilterbankAnalysis::setLowDelayFilterbank(int bufferSize, int nChannels) { static_cast<FilterbankAnalysisWOLAImpl*>(pimpl.get())->algo.setLowDelayFilterbank(bufferSize, nChannels); }
void FilterbankAnalysis::setHighQualityFilterbank(int bufferSize, int nChannels) { static_cast<FilterbankAnalysisWOLAImpl*>(pimpl.get())->algo.setHighQualityFilterbank(bufferSize, nChannels); }

void FilterbankSynthesis::setWindow(I::Real window) { static_cast<FilterbankSynthesisWOLAImpl*>(pimpl.get())->algo.setWindow(window); }
void FilterbankSynthesis::setStandardFilterbank(int bufferSize, int nChannels) { static_cast<FilterbankSynthesisWOLAImpl*>(pimpl.get())->algo.setStandardFilterbank(bufferSize, nChannels); }
void FilterbankSynthesis::setLowDelayFilterbank(int bufferSize, int nChannels) { static_cast<FilterbankSynthesisWOLAImpl*>(pimpl.get())->algo.setLowDelayFilterbank(bufferSize, nChannels); }
void FilterbankSynthesis::setHighQualityFilterbank(int bufferSize, int nChannels) { static_cast<FilterbankSynthesisWOLAImpl*>(pimpl.get())->algo.setHighQualityFilterbank(bufferSize, nChannels); }