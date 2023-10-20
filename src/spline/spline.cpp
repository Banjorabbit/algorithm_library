#include "spline/spline_cubic.h"

template<> 
Algorithm<SplineConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<SplineCubic, SplineConfiguration>>();
} 

template<> 
Algorithm<SplineConfiguration>::~Algorithm() {} 

template<>
Algorithm<SplineConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<SplineCubic, SplineConfiguration>>(c);
} 

 Spline::Spline(const Coefficients& c) : Algorithm<SplineConfiguration>{c} {}