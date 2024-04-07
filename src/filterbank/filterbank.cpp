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