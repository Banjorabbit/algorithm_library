#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "noise_estimation/noise_estimation_activity_detection.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(NoiseEstimationActivityDetection, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<NoiseEstimationActivityDetection>());
}