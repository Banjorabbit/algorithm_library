#include "spectrogram/spectrogram_nonlinear.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(SpectrogramNonlinear, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<SpectrogramNonlinear>()); }

// description: test nFrames is a static function and that it returns correct number of frames.
TEST(SpectrogramNonlinear, getNFrames)
{
    SpectrogramNonlinear spec;
    auto c = spec.getCoefficients();
    const int bufferSize = c.bufferSize;
    const int nSamples = 10000;
    const int nFrames = Spectrogram::Configuration::getNFrames(nSamples, bufferSize); // test getNFrames() is a static method

    ArrayXf signal(nSamples);
    signal.setRandom();

    ArrayXXf output(c.nBands, nFrames + 1); // add one extra frame than needed
    output.setZero();                       // important to set to zero, since we are checking last frame is zero in success criteria
    spec.process(signal, output);

    bool criteria = (!output.leftCols(nFrames).isZero()) && (output.rightCols(1).isZero()); // test criteria that all nFrames are non-zero and last frame is zero
    fmt::print("Criteria: {}\n", criteria);
    EXPECT_TRUE(criteria);
}
