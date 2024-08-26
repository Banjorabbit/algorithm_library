#include "gain_apply/gain_apply_simple.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(GainApplySimple, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<GainApplySimple>()); }
