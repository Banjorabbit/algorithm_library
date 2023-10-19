#include "filter_min_max/filter_min_max_lemire.h"

DEFINE_SOURCE_INTERFACE(StreamingMinMaxConfiguration, StreamingMinMaxLemire)
StreamingMinMax::StreamingMinMax(const Coefficients& c) : Algorithm<StreamingMinMaxConfiguration>{ c } {}
void StreamingMinMax::resetInitialValue(const float inputOld) { pimpl->algo.resetInitialValue(inputOld); }
void StreamingMinMax::resetInitialValue(const I::Real inputOld) { pimpl->algo.resetInitialValue(inputOld); }

DEFINE_SOURCE_INTERFACE(FilterMinMaxConfiguration, FilterMinMaxLemire)
FilterMinMax::FilterMinMax(const Coefficients& c) : Algorithm<FilterMinMaxConfiguration>{ c } {}
void FilterMinMax::resetInitialValue(const float inputOld) { pimpl->algo.resetInitialValue(inputOld); }
void FilterMinMax::resetInitialValue(const I::Real inputOld) { pimpl->algo.resetInitialValue(inputOld); }

DEFINE_SOURCE_INTERFACE(StreamingMaxConfiguration, StreamingMaxLemire)
StreamingMax::StreamingMax(const Coefficients& c) : Algorithm<StreamingMaxConfiguration>{ c } {}
void StreamingMax::resetInitialValue(const float inputOld) { pimpl->algo.resetInitialValue(inputOld); }
void StreamingMax::resetInitialValue(const I::Real inputOld) { pimpl->algo.resetInitialValue(inputOld); }

DEFINE_SOURCE_INTERFACE(StreamingMinConfiguration, StreamingMinLemire)
StreamingMin::StreamingMin(const Coefficients& c) : Algorithm<StreamingMinConfiguration>{ c } {}
void StreamingMin::resetInitialValue(const float inputOld) { pimpl->algo.resetInitialValue(inputOld); }
void StreamingMin::resetInitialValue(const I::Real inputOld) { pimpl->algo.resetInitialValue(inputOld); }

DEFINE_SOURCE_INTERFACE(FilterMaxConfiguration, FilterMaxLemire)
FilterMax::FilterMax(const Coefficients& c) : Algorithm<FilterMaxConfiguration>{ c } {}
void FilterMax::resetInitialValue(const float inputOld) { pimpl->algo.resetInitialValue(inputOld); }
void FilterMax::resetInitialValue(const I::Real inputOld) { pimpl->algo.resetInitialValue(inputOld); }

DEFINE_SOURCE_INTERFACE(FilterMinConfiguration, FilterMinLemire)
FilterMin::FilterMin(const Coefficients& c) : Algorithm<FilterMinConfiguration>{ c } {}
void FilterMin::resetInitialValue(const float inputOld) { pimpl->algo.resetInitialValue(inputOld); }
void FilterMin::resetInitialValue(const I::Real inputOld) { pimpl->algo.resetInitialValue(inputOld); }