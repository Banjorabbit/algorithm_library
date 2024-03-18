#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "design_iir_non_parametric/design_iir_spline.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(DesignIIRNonParametric, Interface)
{
	bool testMallocFlag = false;
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<DesignIIRSpline>(testMallocFlag));
}
