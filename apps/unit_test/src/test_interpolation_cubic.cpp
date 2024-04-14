#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "interpolation/interpolation_cubic.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(InterpolationCubic, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<InterpolationCubic>());
}

TEST(InterpolationCubicSample, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<InterpolationCubicSample>());
}

TEST(InterpolationCubicConstant, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<InterpolationCubicConstant>());
}
