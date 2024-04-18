#include "beamformer/beamformer_mvdr.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(BeamformerMVDR, Interface)
{
    bool testMallocFlag = false;
    EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<BeamformerMVDR>(testMallocFlag));
}
