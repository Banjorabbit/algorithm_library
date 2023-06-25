#include "normal3d/normal3d_diff.h"

DEFINE_SOURCE_INTERFACE(Normal3dConfiguration, Normal3dDiff)

 Normal3d::Normal3d(const Coefficients& c) : Algorithm<Normal3dConfiguration>{c} {}