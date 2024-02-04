#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "iir_filter_time_varying/state_variable_filter.h"
#include "iir_filter/iir_filter_2nd_order.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(StateVariableFilter, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<StateVariableFilter>());
}


TEST(StateVariableFilter, InterfaceCascade)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<StateVariableFilterCascade>());
}

// Check that converting the SFV representation of the filter coefficients to sos and back again gives the same filter.
TEST(StateVariableFilter, CheckConversion)
{
	int nBands = 1025;
	using pF = StateVariableFilter::Parameters;

	StateVariableFilterCascade::Coefficients c;
	c.nSos = 7;
	StateVariableFilterCascade filter(c);
	filter.setFilterTypes({pF::LOWPASS, pF::HIGHPASS, pF::BANDPASS, pF::BANDSTOP, pF::PEAKING, pF::LOWSHELF, pF::HIGHSHELF});

	Eigen::ArrayXf cutoff(c.nSos);
	Eigen::ArrayXf gain(c.nSos);
	Eigen::ArrayXf resonance(c.nSos);
	cutoff.setConstant(std::tan(3.14158f * 3214.f / 16000.f)); // cutoff frequency prewarped: tan(pi*f/fs)
	gain.setConstant(0.6f);
	resonance.setConstant(1.3f);

	// get initial power spectrum
	Eigen::ArrayXf powR = filter.getPowerFrequencyResponse(nBands, cutoff, gain, resonance);
	// convert to sos
	Eigen::ArrayXXf sos = filter.getSosFilter(cutoff, gain, resonance);
	// convert back to SVF form
	Eigen::Array3Xf cgr = filter.setUserDefinedFilter(sos);

	// get power spectrum again
	Eigen::ArrayXf powR2 = filter.getPowerFrequencyResponse(nBands, cgr.row(0).transpose(), cgr.row(1).transpose(), cgr.row(2).transpose());

	// compare error
	float error = (powR - powR2).abs2().sum() / powR.abs2().sum();
	fmt::print("Error: {}\n", error);
	EXPECT_LT(error, 1e-10f);
}

// check that the State Variable filter and IIRFilter2ndOrder filter has the same filter response given the same second-order-sections
TEST(StateVariableFilter, ComparetoIIRFilter2ndOrder)
{
	int nBands = 1025;
	using pF = StateVariableFilter::Parameters;

	StateVariableFilterCascade::Coefficients c;
	c.nSos = 7;
	StateVariableFilterCascade filter(c);
	filter.setFilterTypes({pF::LOWPASS, pF::HIGHPASS, pF::BANDPASS, pF::BANDSTOP, pF::PEAKING, pF::LOWSHELF, pF::HIGHSHELF});

	IIRFilterCascaded::Coefficients c2nd;
	c2nd.nSos = c.nSos;
	IIRFilterCascaded filter2nd(c2nd);

	Eigen::ArrayXf cutoff(c.nSos);
	Eigen::ArrayXf gain(c.nSos);
	Eigen::ArrayXf resonance(c.nSos);
	cutoff.setConstant(std::tan(3.14158f * 3214.f / 16000.f)); // cutoff frequency prewarped: tan(pi*f/fs)
	gain.setConstant(0.6f);
	resonance.setConstant(1.3f);

	// get initial power spectrum
	Eigen::ArrayXf powR = filter.getPowerFrequencyResponse(nBands, cutoff, gain, resonance);
	// convert to sos
	Eigen::ArrayXXf sos = filter.getSosFilter(cutoff, gain, resonance);
	
	filter2nd.setFilter(sos, 1.f);
	// get power spectrum of filter2nd
	Eigen::ArrayXf powR2 = filter2nd.getPowerFrequencyResponse(nBands);

	// compare error
	float error = (powR - powR2).abs2().sum() / powR.abs2().sum();
	fmt::print("Error: {}\n", error);
	EXPECT_LT(error, 1e-10f);
}