#include "interpolation/interpolation_cubic.h"

template<> 
Algorithm<InterpolationSampleConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<InterpolationCubicSample, InterpolationSampleConfiguration>>();
} 

template<> 
Algorithm<InterpolationSampleConfiguration>::~Algorithm() {} 

template<>
Algorithm<InterpolationSampleConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<InterpolationCubicSample, InterpolationSampleConfiguration>>(c);
} 

template<> 
Algorithm<InterpolationConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<InterpolationCubic, InterpolationConfiguration>>();
} 

template<> 
Algorithm<InterpolationConfiguration>::~Algorithm() {} 

template<>
Algorithm<InterpolationConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<InterpolationCubic, InterpolationConfiguration>>(c);
} 

template<> 
Algorithm<InterpolationConstantConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<InterpolationCubicConstant, InterpolationConstantConfiguration>>();
} 

template<> 
Algorithm<InterpolationConstantConfiguration>::~Algorithm() {} 

template<>
Algorithm<InterpolationConstantConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<InterpolationCubicConstant, InterpolationConstantConfiguration>>(c);
} 

InterpolationConstant::InterpolationConstant(const Coefficients& c) : Algorithm<InterpolationConstantConfiguration>{c} {}