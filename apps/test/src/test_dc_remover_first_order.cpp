#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "dc_remover/dc_remover_first_order.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(DCRemover, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<DCRemoverFirstOrder>());
}
