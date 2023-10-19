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

	FilterbankAnalysis filterbank;
	filterbank.setHighQualityFilterbank();

	FilterbankSynthesis filterbankInv;
	filterbankInv.setHighQualityFilterbank();

	auto c = filterbankInv.getCoefficients();

	ArrayXXf input(nFrames * c.bufferSize, c.nChannels);
	input.setRandom();
	
	ArrayXXcf outFreq = filterbank.initOutput(input.topRows(c.bufferSize));
	auto output = ArrayXXf(c.bufferSize * nFrames, c.nChannels); 
	for (auto i = 0; i < nFrames; i++)
	{
		filterbank.process(input.middleRows(i * c.bufferSize, c.bufferSize), outFreq);
		filterbankInv.process(outFreq, output.middleRows(i * c.bufferSize, c.bufferSize));
	}
	int offset = c.frameSize / c.bufferSize - 1;
	float error = (input.topRows((nFrames - offset) * c.bufferSize) - output.bottomRows((nFrames - offset) * c.bufferSize)).abs2().mean();
	error /= input.topRows((nFrames - offset) * c.bufferSize).abs2().mean();

	fmt::print("Output error: {}\n", error);
	EXPECT_LT(error, 1e-6f);
}

// Description: Send random signal through LowDelay filterbank and reconstruct it.
// pass/fail: check reconstruction error is below threshold.
TEST(Filterbank, ReconstructionLowDelay)
{
	const int nFrames = 100; // number of frames to process

	FilterbankAnalysis filterbank;
	filterbank.setLowDelayFilterbank();

	FilterbankSynthesis filterbankInv;
	filterbankInv.setLowDelayFilterbank();

	auto c = filterbankInv.getCoefficients();

	ArrayXXf input(nFrames * c.bufferSize, c.nChannels);
	input.setRandom();
	ArrayXXf output(nFrames * c.bufferSize, c.nChannels);

	auto outFreq = filterbank.initOutput(input.topRows(c.bufferSize));
	for (auto i = 0; i < nFrames; i++)
	{
		filterbank.process(input.middleRows(i * c.bufferSize, c.bufferSize), outFreq);
		filterbankInv.process(outFreq, output.middleRows(i * c.bufferSize, c.bufferSize));
	}
	int offset = c.frameSize / c.bufferSize - 1;
	float error = (input.topRows((nFrames - offset) * c.bufferSize) - output.bottomRows((nFrames - offset) * c.bufferSize)).abs2().mean();
	error /= input.topRows((nFrames - offset) * c.bufferSize).abs2().mean();

	fmt::print("Output error: {}\n", error);
	EXPECT_LT(error, 1e-6f);
}

// Description: Send random signal through Standard filterbank and reconstruct it.
// pass/fail: check reconstruction error is below threshold.
TEST(Filterbank, ReconstructionStandard)
{
	const int nFrames = 100; // number of frames to process

	FilterbankAnalysis filterbank;
	filterbank.setStandardFilterbank();

	FilterbankSynthesis filterbankInv;
	filterbankInv.setStandardFilterbank();

	auto c = filterbankInv.getCoefficients();

	ArrayXXf input(nFrames * c.bufferSize, c.nChannels);
	input.setRandom();
	ArrayXXf output(nFrames * c.bufferSize, c.nChannels);

	auto outFreq = filterbank.initOutput(input.topRows(c.bufferSize));
	for (auto i = 0; i < nFrames; i++)
	{
		filterbank.process(input.middleRows(i * c.bufferSize, c.bufferSize), outFreq);
		filterbankInv.process(outFreq, output.middleRows(i * c.bufferSize, c.bufferSize));
	}
	int offset = c.frameSize / c.bufferSize - 1;
	float error = (input.topRows((nFrames - offset) * c.bufferSize) - output.bottomRows((nFrames - offset) * c.bufferSize)).abs2().mean();
	error /= input.topRows((nFrames - offset) * c.bufferSize).abs2().mean();

	fmt::print("Output error: {}\n", error);
	EXPECT_LT(error, 1e-6f);
}