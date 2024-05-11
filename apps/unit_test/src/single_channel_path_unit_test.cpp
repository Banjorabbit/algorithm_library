#include "single_channel_path/noise_reduction_path.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(SingleChannelPath, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<NoiseReductionPath>()); }
