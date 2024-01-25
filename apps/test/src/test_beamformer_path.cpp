#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "preprocessing_path/beamformer_path.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(BeamformerPath, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<BeamformerPath>());
}

