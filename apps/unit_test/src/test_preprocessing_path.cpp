#include "framework/unit_test.h"
#include "preprocessing_path/beamformer_path.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(PreprocessingPath, Interface)
{
    bool testMallocFlag = false;
    EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<BeamformerPath>(testMallocFlag));
}

// test that bufferMode is correctly changed to asynchronous mode
TEST(PreprocessingPath, SetAsynchronousMode)
{
    PreprocessingPath prePath;
    prePath.setBufferMode(BufferMode::ASYNCHRONOUS_BUFFER);
    auto mode = prePath.getBufferMode();
    EXPECT_TRUE(mode == BufferMode::ASYNCHRONOUS_BUFFER);
}

// test that coefficients are saved when changing buffer mode
TEST(PreprocessingPath, CoefficientsSetBufferMode)
{
    PreprocessingPath prePath;

    auto c = prePath.getCoefficients();
    c.bufferSize = c.bufferSize * 2;
    c.nChannels = c.nChannels * 2;
    prePath.setCoefficients(c);

    prePath.setBufferMode(BufferMode::ASYNCHRONOUS_BUFFER);

    auto cNew = prePath.getCoefficients();
    auto mode = prePath.getBufferMode();

    EXPECT_TRUE(c.bufferSize == cNew.bufferSize);
    EXPECT_TRUE(c.nChannels == cNew.nChannels);
    EXPECT_TRUE(mode == BufferMode::ASYNCHRONOUS_BUFFER);
}
