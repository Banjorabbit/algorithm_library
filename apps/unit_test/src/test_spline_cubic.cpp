#include "framework/unit_test.h"
#include "spline/spline_cubic.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(SplineCubic, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<SplineCubic>()); }
