#include "bandsplit_downsample/bandsplit_downsample_chebyshev.h"
#include "fft/fft_real.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(BandsplitDownsampleChebyshev, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<BandsplitDownsampleChebyshev>()); }

TEST(CombineBandsplitDownsampleChebyshev, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<CombineBandsplitDownsampleChebyshev>()); }

// bandsplit, downsample and combine a random signal. Check reconstruction error is below limit
TEST(CombineBandsplitDownsampleChebyshev, reconstruction)
{
    BandsplitDownsampleChebyshev::Coefficients cBandsplit;
    cBandsplit.nChannels = 2;
    cBandsplit.nSamples = 9999;
    cBandsplit.resamplingType = cBandsplit.K48HZ_TO_K16HZ;
    BandsplitDownsampleChebyshev bandsplit(cBandsplit);

    CombineBandsplitDownsampleChebyshev::Coefficients cCombine;
    cCombine.nChannels = cBandsplit.nChannels;
    cCombine.nSamples = cBandsplit.nSamples / 3;
    cCombine.resamplingType = cCombine.K16HZ_TO_K48HZ;
    CombineBandsplitDownsampleChebyshev combineBandsplit(cCombine);

    Eigen::ArrayXXf x = Eigen::ArrayXXf::Random(cBandsplit.nSamples, cBandsplit.nChannels);
    Eigen::ArrayXXf yDownsample = Eigen::ArrayXXf::Zero(cCombine.nSamples, cCombine.nChannels);
    Eigen::ArrayXXf yHighFrequency = Eigen::ArrayXXf::Zero(cBandsplit.nSamples, cCombine.nChannels);
    Eigen::ArrayXXf y = Eigen::ArrayXXf::Zero(cBandsplit.nSamples, cBandsplit.nChannels);

    bandsplit.process(x, {yDownsample, yHighFrequency});
    combineBandsplit.process({yDownsample, yHighFrequency}, y);

    // calculate spectrum of output signals and calculate output error
    const int delay = 13;                // delay between input/output
    int n = cBandsplit.nSamples - delay; // length of test signal

    // init FFT
    FFTReal::Coefficients cFFT;
    cFFT.fftSize = FFTReal::Configuration::getValidFFTSize(cBandsplit.nSamples);
    FFTReal fft(cFFT);

    // init FFT arrays
    Eigen::ArrayXXf xZeropad = Eigen::ArrayXXf::Zero(cFFT.fftSize, cBandsplit.nChannels);
    xZeropad.topRows(n) = x.topRows(n);
    Eigen::ArrayXXf yZeropad = Eigen::ArrayXXf::Zero(cFFT.fftSize, cBandsplit.nChannels);
    yZeropad.topRows(n) = y.bottomRows(n);

    Eigen::ArrayXXcf X(FFTReal::Configuration::convertFFTSizeToNBands(cFFT.fftSize), cBandsplit.nChannels);
    Eigen::ArrayXXcf Y(FFTReal::Configuration::convertFFTSizeToNBands(cFFT.fftSize), cBandsplit.nChannels);

    // calculate FFT and print error
    fft.process(xZeropad, X);
    fft.process(yZeropad, Y);

    float error = (X.abs() - Y.abs()).abs2().sum() / X.abs2().sum();
    fmt::print("Output error: {}\n", error);
    ASSERT_TRUE(error < 0.0005);
}