#include "normal3d/normal3d_diff.h"

template<> 
Algorithm<Normal3dConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<Normal3dDiff, Normal3dConfiguration>>();
} 

template<> 
Algorithm<Normal3dConfiguration>::~Algorithm() {} 

template<>
Algorithm<Normal3dConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<Normal3dDiff, Normal3dConfiguration>>(c);
} 

Normal3d::Normal3d(const Coefficients& c) : Algorithm<Normal3dConfiguration>{c} {}