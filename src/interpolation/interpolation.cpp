#include "interpolation/interpolation_cubic.h"

DEFINE_SOURCE_INTERFACE(InterpolationSampleConfiguration, InterpolationCubicSample)

DEFINE_SOURCE_INTERFACE(InterpolationConfiguration, InterpolationCubic)

DEFINE_SOURCE_INTERFACE(InterpolationConstantConfiguration, InterpolationCubicConstant)

 InterpolationConstant::InterpolationConstant(const Coefficients& c) : Algorithm<InterpolationConstantConfiguration>{c} {}