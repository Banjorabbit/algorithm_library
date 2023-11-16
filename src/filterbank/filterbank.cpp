#include "filterbank/filterbank_wola.h"

DEFINE_CONSTRUCTOR_DESTRUCTOR(FilterbankAnalysis, FilterbankAnalysisWOLA, FilterbankAnalysisConfiguration)
DEFINE_CONSTRUCTOR_DESTRUCTOR(FilterbankSynthesis, FilterbankSynthesisWOLA, FilterbankSynthesisConfiguration)

void FilterbankAnalysis::setWindow(I::Real window) { static_cast<FilterbankAnalysisWOLASingleBufferImpl*>(pimpl.get())->algo.setWindow(window); }
void FilterbankAnalysis::setStandardFilterbank(int bufferSize) { static_cast<FilterbankAnalysisWOLASingleBufferImpl*>(pimpl.get())->algo.setStandardFilterbank(bufferSize); }
void FilterbankAnalysis::setLowDelayFilterbank(int bufferSize) { static_cast<FilterbankAnalysisWOLASingleBufferImpl*>(pimpl.get())->algo.setLowDelayFilterbank(bufferSize); }
void FilterbankAnalysis::setHighQualityFilterbank(int bufferSize) { static_cast<FilterbankAnalysisWOLASingleBufferImpl*>(pimpl.get())->algo.setHighQualityFilterbank(bufferSize); }

void FilterbankSynthesis::setWindow(I::Real window) { static_cast<FilterbankSynthesisWOLASingleBufferImpl*>(pimpl.get())->algo.setWindow(window); }
void FilterbankSynthesis::setStandardFilterbank(int bufferSize) { static_cast<FilterbankSynthesisWOLASingleBufferImpl*>(pimpl.get())->algo.setStandardFilterbank(bufferSize); }
void FilterbankSynthesis::setLowDelayFilterbank(int bufferSize) { static_cast<FilterbankSynthesisWOLASingleBufferImpl*>(pimpl.get())->algo.setLowDelayFilterbank(bufferSize); }
void FilterbankSynthesis::setHighQualityFilterbank(int bufferSize) { static_cast<FilterbankSynthesisWOLASingleBufferImpl*>(pimpl.get())->algo.setHighQualityFilterbank(bufferSize); }