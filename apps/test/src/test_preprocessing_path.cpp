#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "preprocessing_path/beamformer_path.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(PreprocessingPath, Interface)
{
    bool testMallocFlag = false;
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<BeamformerPath>(testMallocFlag));
}

TEST(PreprocessingPath, Single)
{
    PreprocessingPath prePath;

    auto c = prePath.getCoefficients();
    c.nChannels = 1;
    c.bufferMode = ASYNCHRONOUS_BUFFER;
    prePath.setCoefficients(c);
}