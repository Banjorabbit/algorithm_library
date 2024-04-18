#include "iir_filter/iir_filter_2nd_order.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(IIRFilter, Interface2ndOrder) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<IIRFilter2ndOrder>()); }

TEST(IIRFilter, InterfaceCascaded) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<IIRFilterCascaded>()); }
