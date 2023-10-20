#include "filterbank/filterbank_wola.h"

template<> 
Algorithm<FilterbankAnalysisConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<FilterbankAnalysisWOLA, FilterbankAnalysisConfiguration>>();
} 

template<> 
Algorithm<FilterbankAnalysisConfiguration>::~Algorithm() {} 

template<>
Algorithm<FilterbankAnalysisConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<FilterbankAnalysisWOLA, FilterbankAnalysisConfiguration>>(c);
} 

FilterbankAnalysis::FilterbankAnalysis(const Coefficients& c) : Algorithm<FilterbankAnalysisConfiguration>{ c } {}
void FilterbankAnalysis::setWindow(I::Real window) { static_cast<Impl<FilterbankAnalysisWOLA, FilterbankAnalysisConfiguration>*>(pimpl.get())->algo.setWindow(window); }
void FilterbankAnalysis::setStandardFilterbank(int bufferSize, int nChannels) { static_cast<Impl<FilterbankAnalysisWOLA, FilterbankAnalysisConfiguration>*>(pimpl.get())->algo.setStandardFilterbank(bufferSize, nChannels); }
void FilterbankAnalysis::setLowDelayFilterbank(int bufferSize, int nChannels) { static_cast<Impl<FilterbankAnalysisWOLA, FilterbankAnalysisConfiguration>*>(pimpl.get())->algo.setLowDelayFilterbank(bufferSize, nChannels); }
void FilterbankAnalysis::setHighQualityFilterbank(int bufferSize, int nChannels) { static_cast<Impl<FilterbankAnalysisWOLA, FilterbankAnalysisConfiguration>*>(pimpl.get())->algo.setHighQualityFilterbank(bufferSize, nChannels); }

template<> 
Algorithm<FilterbankSynthesisConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<FilterbankSynthesisWOLA, FilterbankSynthesisConfiguration>>();
} 

template<> 
Algorithm<FilterbankSynthesisConfiguration>::~Algorithm() {} 

template<>
Algorithm<FilterbankSynthesisConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<FilterbankSynthesisWOLA, FilterbankSynthesisConfiguration>>(c);
} 


FilterbankSynthesis::FilterbankSynthesis(const Coefficients& c) : Algorithm<FilterbankSynthesisConfiguration>{ c } {}
void FilterbankSynthesis::setWindow(I::Real window) { static_cast<Impl<FilterbankSynthesis, FilterbankSynthesisConfiguration>*>(pimpl.get())->algo.setWindow(window); }
void FilterbankSynthesis::setStandardFilterbank(int bufferSize, int nChannels) { static_cast<Impl<FilterbankSynthesisWOLA, FilterbankSynthesisConfiguration>*>(pimpl.get())->algo.setStandardFilterbank(bufferSize, nChannels); }
void FilterbankSynthesis::setLowDelayFilterbank(int bufferSize, int nChannels) { static_cast<Impl<FilterbankSynthesisWOLA, FilterbankSynthesisConfiguration>*>(pimpl.get())->algo.setLowDelayFilterbank(bufferSize, nChannels); }
void FilterbankSynthesis::setHighQualityFilterbank(int bufferSize, int nChannels) { static_cast<Impl<FilterbankSynthesisWOLA, FilterbankSynthesisConfiguration>*>(pimpl.get())->algo.setHighQualityFilterbank(bufferSize, nChannels); }