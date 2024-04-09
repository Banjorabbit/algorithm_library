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

TEST(PreprocessingPath, SetAsynchronousMode)
{
    PreprocessingPath prePath;
    prePath.setBufferMode(BufferMode::ASYNCHRONOUS_BUFFER);
    auto mode = prePath.getBufferMode();
    EXPECT_TRUE(mode == BufferMode::ASYNCHRONOUS_BUFFER);
}