#include "filter_min_max/filter_min_max_lemire.h"

//DEFINE_SOURCE_INTERFACE(StreamingMinMaxConfiguration, StreamingMinMaxLemire)

template<> 
Algorithm<StreamingMinMaxConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<StreamingMinMaxLemire, StreamingMinMaxConfiguration>>();
} 

template<> 
Algorithm<StreamingMinMaxConfiguration>::~Algorithm() {} 

template<>
Algorithm<StreamingMinMaxConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<StreamingMinMaxLemire, StreamingMinMaxConfiguration>>(c);
} 

StreamingMinMax::StreamingMinMax(const Coefficients& c) : Algorithm<StreamingMinMaxConfiguration>{ c } {}
void StreamingMinMax::resetInitialValue(const float inputOld) { static_cast<Impl<StreamingMinMaxLemire, StreamingMinMaxConfiguration>*>(pimpl.get())->algo.resetInitialValue(inputOld); }
void StreamingMinMax::resetInitialValue(const I::Real inputOld) { static_cast<Impl<StreamingMinMaxLemire, StreamingMinMaxConfiguration>*>(pimpl.get())->algo.resetInitialValue(inputOld); }

//DEFINE_SOURCE_INTERFACE(FilterMinMaxConfiguration, FilterMinMaxLemire)

template<> 
Algorithm<FilterMinMaxConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<FilterMinMaxLemire, FilterMinMaxConfiguration>>();
} 

template<> 
Algorithm<FilterMinMaxConfiguration>::~Algorithm() {} 

template<>
Algorithm<FilterMinMaxConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<FilterMinMaxLemire, FilterMinMaxConfiguration>>(c);
} 

FilterMinMax::FilterMinMax(const Coefficients& c) : Algorithm<FilterMinMaxConfiguration>{ c } {}
void FilterMinMax::resetInitialValue(const float inputOld) { static_cast<Impl<FilterMinMaxLemire, FilterMinMaxConfiguration>*>(pimpl.get())->algo.resetInitialValue(inputOld); }
void FilterMinMax::resetInitialValue(const I::Real inputOld) { static_cast<Impl<FilterMinMaxLemire, FilterMinMaxConfiguration>*>(pimpl.get())->algo.resetInitialValue(inputOld); }

//DEFINE_SOURCE_INTERFACE(StreamingMaxConfiguration, StreamingMaxLemire)

template<> 
Algorithm<StreamingMaxConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<StreamingMaxLemire, StreamingMaxConfiguration>>();
} 

template<> 
Algorithm<StreamingMaxConfiguration>::~Algorithm() {} 

template<>
Algorithm<StreamingMaxConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<StreamingMaxLemire, StreamingMaxConfiguration>>(c);
} 

StreamingMax::StreamingMax(const Coefficients& c) : Algorithm<StreamingMaxConfiguration>{ c } {}
void StreamingMax::resetInitialValue(const float inputOld) { static_cast<Impl<StreamingMaxLemire, StreamingMaxConfiguration>*>(pimpl.get())->algo.resetInitialValue(inputOld); }
void StreamingMax::resetInitialValue(const I::Real inputOld) { static_cast<Impl<StreamingMaxLemire, StreamingMaxConfiguration>*>(pimpl.get())->algo.resetInitialValue(inputOld); }


//DEFINE_SOURCE_INTERFACE(StreamingMinConfiguration, StreamingMinLemire)

template<> 
Algorithm<StreamingMinConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<StreamingMinLemire, StreamingMinConfiguration>>();
} 

template<> 
Algorithm<StreamingMinConfiguration>::~Algorithm() {} 

template<>
Algorithm<StreamingMinConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<StreamingMinLemire, StreamingMinConfiguration>>(c);
} 

StreamingMin::StreamingMin(const Coefficients& c) : Algorithm<StreamingMinConfiguration>{ c } {}
void StreamingMin::resetInitialValue(const float inputOld) { static_cast<Impl<StreamingMinLemire, StreamingMinConfiguration>*>(pimpl.get())->algo.resetInitialValue(inputOld); }
void StreamingMin::resetInitialValue(const I::Real inputOld) { static_cast<Impl<StreamingMinLemire, StreamingMinConfiguration>*>(pimpl.get())->algo.resetInitialValue(inputOld); }

template<> 
Algorithm<FilterMaxConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<FilterMaxLemire, FilterMaxConfiguration>>();
} 

template<> 
Algorithm<FilterMaxConfiguration>::~Algorithm() {} 

template<>
Algorithm<FilterMaxConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<FilterMaxLemire, FilterMaxConfiguration>>(c);
} 

FilterMax::FilterMax(const Coefficients& c) : Algorithm<FilterMaxConfiguration>{ c } {}
void FilterMax::resetInitialValue(const float inputOld) { static_cast<Impl<FilterMaxLemire, FilterMaxConfiguration>*>(pimpl.get())->algo.resetInitialValue(inputOld); }
void FilterMax::resetInitialValue(const I::Real inputOld) { static_cast<Impl<FilterMaxLemire, FilterMaxConfiguration>*>(pimpl.get())->algo.resetInitialValue(inputOld); }

template<> 
Algorithm<FilterMinConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<FilterMinLemire, FilterMinConfiguration>>();
} 

template<> 
Algorithm<FilterMinConfiguration>::~Algorithm() {} 

template<>
Algorithm<FilterMinConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<FilterMinLemire, FilterMinConfiguration>>(c);
} 

FilterMin::FilterMin(const Coefficients& c) : Algorithm<FilterMinConfiguration>{ c } {}
void FilterMin::resetInitialValue(const float inputOld) { static_cast<Impl<FilterMinLemire, FilterMinConfiguration>*>(pimpl.get())->algo.resetInitialValue(inputOld); }
void FilterMin::resetInitialValue(const I::Real inputOld) { static_cast<Impl<FilterMinLemire, FilterMinConfiguration>*>(pimpl.get())->algo.resetInitialValue(inputOld); }
