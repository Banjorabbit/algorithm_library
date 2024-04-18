#include "design_iir_non_parametric/design_iir_spline.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(DesignIIRNonParametric, Interface)
{
    bool testMallocFlag = false;
    EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<DesignIIRSpline>(testMallocFlag));
}
