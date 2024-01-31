#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "iir_filter_time_varying/state_variable_filter.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(StateVariableFilter, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<StateVariableFilter>());
}


TEST(StateVariableFilter, InterfaceCascade)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<StateVariableFilterCascade>());
}