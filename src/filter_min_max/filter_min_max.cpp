#include "filter_min_max/filter_min_max_lemire.h"

DEFINE_SOURCE_INTERFACE(StreamingMinMaxConfiguration, StreamingMinMax)

StreamingMinMax::StreamingMinMax(const Coefficients& c) : Algorithm<StreamingMinMaxConfiguration>{ c } {}
void StreamingMinMax::resetInitialValue(const float inputOld) { pimpl->algo.resetInitialValue(inputOld); }
void StreamingMinMax::resetInitialValue(const I::Real inputOld) { pimpl->algo.resetInitialValue(inputOld); }

DEFINE_SOURCE_INTERFACE(FilterMinMaxConfiguration, FilterMinMax)

FilterMinMax::FilterMinMax(const Coefficients& c) : Algorithm<FilterMinMaxConfiguration>{ c } {}
void FilterMinMax::resetInitialValue(const float inputOld) { pimpl->algo.resetInitialValue(inputOld); }
void FilterMinMax::resetInitialValue(const I::Real inputOld) { pimpl->algo.resetInitialValue(inputOld); }
