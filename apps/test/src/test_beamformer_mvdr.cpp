#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "beamformer/beamformer_mvdr.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(BeamformerMVDR, Interface)
{
	bool testMallocFlag = false;
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<BeamformerMVDR>(testMallocFlag));
}

