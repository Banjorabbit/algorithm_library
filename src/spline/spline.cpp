#include "spline/spline_cubic.h"

DEFINE_SOURCE_INTERFACE(SplineConfiguration, SplineCubic)

 Spline::Spline(const Coefficients& c) : Algorithm<SplineConfiguration>{c} {}