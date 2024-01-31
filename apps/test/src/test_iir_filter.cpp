#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "iir_filter/iir_filter_2nd_order.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(IIRFilter, Interface2ndOrder)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<IIRFilter2ndOrder>());
}

TEST(IIRFilter, InterfaceCascaded)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<IIRFilterCascaded>());
}

// test the default power frequency response is flat, also for a cascaded filter
TEST(IIRFilter, PowerFrequencyResponse)
{
	IIRFilter filter;
	int nBands=257;
	Eigen::ArrayXf powR = filter.getPowerFrequencyResponse(nBands);

	float error = (powR-1).abs2().sum() / nBands;
	fmt::print("powR error: {}\n", error);
	EXPECT_TRUE(error< 1e-12f);

	auto c = filter.getCoefficients();
	c.nSos = 10;
	Eigen::ArrayXf powRCascaded = filter.getPowerFrequencyResponse(nBands);

	error = (powR - powRCascaded).abs2().sum();
	fmt::print("powR Cascaded: {}\n", error);
	EXPECT_TRUE(error == 0);
}