#include "spline/spline_cubic.h"

DEFINE_SOURCE_INTERFACE(SplineConfiguration, SplineCubic)

 Spline::Spline(const Parameters& p) : Algorithm<SplineConfiguration>{p} {}