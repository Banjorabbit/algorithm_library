#include "filter_min_max/filter_min_max_lemire.h"

DEFINE_CONSTRUCTOR_DESTRUCTOR(StreamingMinMax, StreamingMinMaxLemire, StreamingMinMaxConfiguration)
DEFINE_CONSTRUCTOR_DESTRUCTOR(FilterMinMax, FilterMinMaxLemire, FilterMinMaxConfiguration)
DEFINE_CONSTRUCTOR_DESTRUCTOR(StreamingMax, StreamingMaxLemire, StreamingMaxConfiguration)
DEFINE_CONSTRUCTOR_DESTRUCTOR(StreamingMin, StreamingMinLemire, StreamingMinConfiguration)
DEFINE_CONSTRUCTOR_DESTRUCTOR(FilterMax, FilterMaxLemire, FilterMaxConfiguration)
DEFINE_CONSTRUCTOR_DESTRUCTOR(FilterMin, FilterMinLemire, FilterMinConfiguration)

void StreamingMinMax::resetInitialValue(const float inputOld) { static_cast<StreamingMinMaxLemireImpl*>(pimpl.get())->algo.resetInitialValue(inputOld); }
void StreamingMinMax::resetInitialValue(const I::Real inputOld) { static_cast<StreamingMinMaxLemireImpl*>(pimpl.get())->algo.resetInitialValue(inputOld); }

void FilterMinMax::resetInitialValue(const float inputOld) { static_cast<FilterMinMaxLemireImpl*>(pimpl.get())->algo.resetInitialValue(inputOld); }
void FilterMinMax::resetInitialValue(const I::Real inputOld) { static_cast<FilterMinMaxLemireImpl*>(pimpl.get())->algo.resetInitialValue(inputOld); }

void StreamingMax::resetInitialValue(const float inputOld) { static_cast<StreamingMaxLemireImpl*>(pimpl.get())->algo.resetInitialValue(inputOld); }
void StreamingMax::resetInitialValue(const I::Real inputOld) { static_cast<StreamingMaxLemireImpl*>(pimpl.get())->algo.resetInitialValue(inputOld); }

void StreamingMin::resetInitialValue(const float inputOld) { static_cast<StreamingMinLemireImpl*>(pimpl.get())->algo.resetInitialValue(inputOld); }
void StreamingMin::resetInitialValue(const I::Real inputOld) { static_cast<StreamingMinLemireImpl*>(pimpl.get())->algo.resetInitialValue(inputOld); }

void FilterMax::resetInitialValue(const float inputOld) { static_cast<FilterMaxLemireImpl*>(pimpl.get())->algo.resetInitialValue(inputOld); }
void FilterMax::resetInitialValue(const I::Real inputOld) { static_cast<FilterMaxLemireImpl*>(pimpl.get())->algo.resetInitialValue(inputOld); }

void FilterMin::resetInitialValue(const float inputOld) { static_cast<FilterMinLemireImpl*>(pimpl.get())->algo.resetInitialValue(inputOld); }
void FilterMin::resetInitialValue(const I::Real inputOld) { static_cast<FilterMinLemireImpl*>(pimpl.get())->algo.resetInitialValue(inputOld); }
