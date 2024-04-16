#include "filter_min_max/filter_min_max_lemire.h"

DEFINE_ALGORITHM_CONSTRUCTOR(StreamingMinMax, StreamingMinMaxLemire, StreamingMinMaxConfiguration)
DEFINE_ALGORITHM_CONSTRUCTOR(FilterMinMax, FilterMinMaxLemire, FilterMinMaxConfiguration)
DEFINE_ALGORITHM_CONSTRUCTOR(StreamingMax, StreamingMaxLemire, StreamingMaxConfiguration)
DEFINE_ALGORITHM_CONSTRUCTOR(StreamingMin, StreamingMinLemire, StreamingMinConfiguration)
DEFINE_ALGORITHM_CONSTRUCTOR(FilterMax, FilterMaxLemire, FilterMaxConfiguration)
DEFINE_ALGORITHM_CONSTRUCTOR(FilterMin, FilterMinLemire, FilterMinConfiguration)

void StreamingMinMax::resetInitialValue(const float inputOld) { static_cast<StreamingMinMaxLemireSingleBufferImpl *>(pimpl.get())->algo.resetInitialValue(inputOld); }
void StreamingMinMax::resetInitialValue(I::Real inputOld) { static_cast<StreamingMinMaxLemireSingleBufferImpl *>(pimpl.get())->algo.resetInitialValue(inputOld); }

void FilterMinMax::resetInitialValue(const float inputOld) { static_cast<FilterMinMaxLemireSingleBufferImpl *>(pimpl.get())->algo.resetInitialValue(inputOld); }
void FilterMinMax::resetInitialValue(I::Real inputOld) { static_cast<FilterMinMaxLemireSingleBufferImpl *>(pimpl.get())->algo.resetInitialValue(inputOld); }

void StreamingMax::resetInitialValue(const float inputOld) { static_cast<StreamingMaxLemireSingleBufferImpl *>(pimpl.get())->algo.resetInitialValue(inputOld); }
void StreamingMax::resetInitialValue(I::Real inputOld) { static_cast<StreamingMaxLemireSingleBufferImpl *>(pimpl.get())->algo.resetInitialValue(inputOld); }

void StreamingMin::resetInitialValue(const float inputOld) { static_cast<StreamingMinLemireSingleBufferImpl *>(pimpl.get())->algo.resetInitialValue(inputOld); }
void StreamingMin::resetInitialValue(I::Real inputOld) { static_cast<StreamingMinLemireSingleBufferImpl *>(pimpl.get())->algo.resetInitialValue(inputOld); }

void FilterMax::resetInitialValue(const float inputOld) { static_cast<FilterMaxLemireSingleBufferImpl *>(pimpl.get())->algo.resetInitialValue(inputOld); }
void FilterMax::resetInitialValue(I::Real inputOld) { static_cast<FilterMaxLemireSingleBufferImpl *>(pimpl.get())->algo.resetInitialValue(inputOld); }

void FilterMin::resetInitialValue(const float inputOld) { static_cast<FilterMinLemireSingleBufferImpl *>(pimpl.get())->algo.resetInitialValue(inputOld); }
void FilterMin::resetInitialValue(I::Real inputOld) { static_cast<FilterMinLemireSingleBufferImpl *>(pimpl.get())->algo.resetInitialValue(inputOld); }
