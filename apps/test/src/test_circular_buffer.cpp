#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "circular_buffer/circular_buffer_general.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(CircularBufferGeneral, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<CircularBufferGeneral>());
}

TEST(CircularBufferSingleChannel, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<CircularBufferSingleChannel>());
}
