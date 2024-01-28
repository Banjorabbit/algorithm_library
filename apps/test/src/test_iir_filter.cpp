#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "iir_filter/iir_filter_2nd_order.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(IIRFilter, Interface2ndOrder)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<IIRFilter2ndOrder>());
}

TEST(IIRFilter, InterfaceCascaded)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<IIRFilterCascaded>());
}
