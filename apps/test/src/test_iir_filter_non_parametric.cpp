#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "iir_filter_non_parametric/iir_filter_design_non_parametric.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(IIRFilterNonParametric, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<IIRFilterTDFNonParametric>());
}
