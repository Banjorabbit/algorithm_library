#include "multi_channel_noise_reduction/beamformer_gain_reduction.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(BeamformerGainReduction, Interface)
{
    bool testMallocFlag = false;
    EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<BeamformerGainReduction>(testMallocFlag));
}
