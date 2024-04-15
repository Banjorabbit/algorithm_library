#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "filterbank/filterbank_wola.h"
using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(Filterbank, InterfaceAnalysis) 
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<FilterbankAnalysisWOLA>());
}

TEST(Filterbank, InterfaceSynthesis)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<FilterbankSynthesisWOLA>());
}


// Description: Send random signal through HighQuality filterbank and reconstruct it.
// pass/fail: check reconstruction error is below threshold.
TEST(Filterbank, ReconstructionHighQuality)
{
	const int nFrames = 100; // number of frames to process

	auto c = FilterbankAnalysis::Coefficients();
	c.filterbankType = c.WOLA;
	c.nBands = FFTConfiguration::convertFFTSizeToNBands(4 * c.bufferSize);
	FilterbankAnalysis filterbank(c);

	auto cInv = FilterbankSynthesis::Coefficients();
	cInv.filterbankType = cInv.WOLA;
	cInv.nBands = FFTConfiguration::convertFFTSizeToNBands(4 * cInv.bufferSize);
	FilterbankSynthesis filterbankInv(cInv);

	ArrayXXf input(nFrames * c.bufferSize, c.nChannels);
	input.setRandom();
	
	ArrayXXcf outFreq = filterbank.initOutput(input.topRows(c.bufferSize));
	auto output = ArrayXXf(c.bufferSize * nFrames, c.nChannels); 
	for (auto i = 0; i < nFrames; i++)
	{
		filterbank.process(input.middleRows(i * c.bufferSize, c.bufferSize), outFreq);
		filterbankInv.process(outFreq, output.middleRows(i * c.bufferSize, c.bufferSize));
	}
	int offset = 7 * c.bufferSize; // frameSize - bufferSize = 8 * bufferSize - bufferSize
	float error = (input.topRows(nFrames * c.bufferSize - offset) - output.bottomRows(nFrames * c.bufferSize - offset)).abs2().mean();
	error /= input.topRows(nFrames * c.bufferSize - offset).abs2().mean();

	fmt::print("Output error: {}\n", error);
	EXPECT_LT(error, 1e-6f);
}

// Description: Send random signal through Sqrt_Hann filterbank and reconstruct it.
// pass/fail: check reconstruction error is below threshold.
TEST(Filterbank, ReconstructionSqrtHann)
{
	const int nFrames = 100; // number of frames to process

	auto c = FilterbankAnalysis::Coefficients();
	c.filterbankType = c.SQRT_HANN;
	c.nBands = FFTConfiguration::convertFFTSizeToNBands(2 * c.bufferSize);
	FilterbankAnalysis filterbank(c);

	auto cInv = FilterbankSynthesis::Coefficients();
	cInv.filterbankType = cInv.SQRT_HANN;
	cInv.nBands = FFTConfiguration::convertFFTSizeToNBands(2 * cInv.bufferSize);
	FilterbankSynthesis filterbankInv(cInv);

	ArrayXXf input(nFrames * c.bufferSize, c.nChannels);
	input.setRandom();
	ArrayXXf output(nFrames * c.bufferSize, c.nChannels);

	auto outFreq = filterbank.initOutput(input.topRows(c.bufferSize));
	for (auto i = 0; i < nFrames; i++)
	{
		filterbank.process(input.middleRows(i * c.bufferSize, c.bufferSize), outFreq);
		filterbankInv.process(outFreq, output.middleRows(i * c.bufferSize, c.bufferSize));
	}
	int offset = c.bufferSize; // frameSize - bufferSize = 2 * bufferSize - bufferSize
	float error = (input.topRows(nFrames * c.bufferSize - offset) - output.bottomRows(nFrames * c.bufferSize - offset)).abs2().mean();
	error /= input.topRows(nFrames * c.bufferSize - offset).abs2().mean();

	fmt::print("Output error: {}\n", error);
	EXPECT_LT(error, 1e-6f);
}

// Description: Send random signal through Hann filterbank and reconstruct it.
// pass/fail: check reconstruction error is below threshold.
TEST(Filterbank, ReconstructionStandard)
{
	const int nFrames = 100; // number of frames to process

	auto c = FilterbankAnalysis::Coefficients();
	c.filterbankType = c.HANN;
	c.nBands = FFTConfiguration::convertFFTSizeToNBands(4 * c.bufferSize);
	FilterbankAnalysis filterbank(c);

	auto cInv = FilterbankSynthesis::Coefficients();
	cInv.filterbankType = cInv.HANN;
	cInv.nBands = FFTConfiguration::convertFFTSizeToNBands(4 * cInv.bufferSize);
	FilterbankSynthesis filterbankInv(cInv);

	ArrayXXf input(nFrames * c.bufferSize, c.nChannels);
	input.setRandom();
	ArrayXXf output(nFrames * c.bufferSize, c.nChannels);

	auto outFreq = filterbank.initOutput(input.topRows(c.bufferSize));
	for (auto i = 0; i < nFrames; i++)
	{
		filterbank.process(input.middleRows(i * c.bufferSize, c.bufferSize), outFreq);
		filterbankInv.process(outFreq, output.middleRows(i * c.bufferSize, c.bufferSize));
	}
	int offset = 3 * c.bufferSize; // frameSize - bufferSize = 4 * bufferSize - bufferSize
	float error = (input.topRows(nFrames * c.bufferSize - offset) - output.bottomRows(nFrames * c.bufferSize - offset)).abs2().mean();
	error /= input.topRows(nFrames * c.bufferSize - offset).abs2().mean();

	fmt::print("Output error: {}\n", error);
	EXPECT_LT(error, 1e-6f);
}