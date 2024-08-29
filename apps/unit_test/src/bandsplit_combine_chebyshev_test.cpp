#include "bandsplit_downsample/bandsplit_downsample_chebyshev.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(BandsplitDownsampleChebyshev, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<BandsplitDownsampleChebyshev>()); }

TEST(CombineBandsplitDownsampleChebyshev, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<CombineBandsplitDownsampleChebyshev>()); }