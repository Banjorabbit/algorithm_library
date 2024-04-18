#include "preprocessing_path/beamformer_path.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(BeamformerPath, Interface)
{
    bool testMallocFlag = false;
    EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<BeamformerPath>(testMallocFlag));
}
