#include "framework/unit_test.h"
#include "iir_filter/iir_filter_2nd_order.h"
#include "iir_filter_time_varying/state_variable_filter.h"
#include "gtest/gtest.h"
#include <fmt/ranges.h>
using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(StateVariableFilter, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<StateVariableFilter>()); }

TEST(StateVariableFilter, InterfaceCascade) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<StateVariableFilterCascade>()); }

// Check that converting the SFV representation of the filter coefficients to sos and back to a USER_DEFINED SVF filter gives the same impulse reponse
TEST(StateVariableFilter, CheckConversion)
{
    using pF = StateVariableFilter::Parameters;

    StateVariableFilterCascade::Coefficients c;
    c.nSos = 8;
    c.nChannels = 2;
    StateVariableFilterCascade filter(c);
    filter.setFilterTypes({pF::USER_DEFINED, pF::LOWPASS, pF::HIGHPASS, pF::BANDPASS, pF::BANDSTOP, pF::PEAKING, pF::LOWSHELF, pF::HIGHSHELF});

    Eigen::ArrayXf cutoff(c.nSos);
    Eigen::ArrayXf gain(c.nSos);
    Eigen::ArrayXf resonance(c.nSos);
    cutoff.setConstant(std::tan(3.14158f * 3214.f / 16000.f)); // cutoff frequency prewarped: tan(pi*f/fs)
    gain.setConstant(0.6f);
    resonance.setConstant(1.3f);

    int nSamples = 100;
    Eigen::ArrayXXf input = Eigen::ArrayXXf::Zero(nSamples, c.nChannels);
    input.row(0).setOnes();
    Eigen::ArrayXXf output(nSamples, c.nChannels);
    filter.process({input, cutoff.transpose().replicate(nSamples,1), gain.transpose().replicate(nSamples,1), resonance.transpose().replicate(nSamples,1),}, output);

    // convert to sos
    Eigen::ArrayXXf sos = filter.getSosFilter(cutoff, gain, resonance);
    // convert back to SVF form
    Eigen::Array3Xf cgr = filter.setUserDefinedSosFilter(sos);

    // process filter with new filter
    Eigen::ArrayXXf output2(nSamples, c.nChannels);
    filter.reset();
    filter.process({input, cgr.row(0).replicate(nSamples,1), cgr.row(1).replicate(nSamples,1), cgr.row(2).replicate(nSamples,1),}, output2);

    // check filter output is the same
    float error = (output - output2).abs2().sum() / output.abs2().sum();
    fmt::print("Sample error: {}\n", error);
    EXPECT_LT(error, 1e-10f);

    // compare cutoff, gain and resonance error for USER_DEFINED filter type
    float errorCutoff = std::fabs(cutoff(0) - cgr(0,0)) / cutoff(0);
    float errorGain = std::fabs(gain(0) - cgr(1,0)) / gain(0);
    float errorResonance = std::fabs(resonance(0) - cgr(2,0)) / resonance(0); 
    fmt::print("Cutoff error: {}\n", errorCutoff);
    fmt::print("Gain error: {}\n", errorGain);
    fmt::print("Resonance error: {}\n", errorResonance);
    EXPECT_LT(errorCutoff, 1e-6f);
    EXPECT_LT(errorGain, 1e-6f);
    EXPECT_LT(errorResonance, 1e-6f);

}

// check that the State Variable filter and IIRFilter2ndOrder filter has the same filter response given the same second-order-sections
TEST(StateVariableFilter, ComparetoIIRFilter2ndOrder)
{
    using pF = StateVariableFilter::Parameters;

    StateVariableFilterCascade::Coefficients c;
    c.nSos = 8;
    c.nChannels = 2;
    StateVariableFilterCascade filter(c);
    filter.setFilterTypes({pF::USER_DEFINED, pF::LOWPASS, pF::HIGHPASS, pF::BANDPASS, pF::BANDSTOP, pF::PEAKING, pF::LOWSHELF, pF::HIGHSHELF});

    IIRFilterCascaded::Coefficients c2nd;
    c2nd.nSos = c.nSos;
    c2nd.nChannels = c.nChannels;
    IIRFilterCascaded filter2nd(c2nd);

    Eigen::ArrayXf cutoff(c.nSos);
    Eigen::ArrayXf gain(c.nSos);
    Eigen::ArrayXf resonance(c.nSos);
    cutoff.setConstant(std::tan(3.14158f * 3214.f / 16000.f)); // cutoff frequency prewarped: tan(pi*f/fs)
    gain.setConstant(0.6f);
    resonance.setConstant(1.3f);

    int nSamples = 100;
    Eigen::ArrayXXf input = Eigen::ArrayXXf::Zero(nSamples, c.nChannels);
    input.row(0).setOnes();

    // convert to sos
    Eigen::ArrayXXf sos = filter.getSosFilter(cutoff, gain, resonance);
    filter2nd.setFilter(sos, 1.f);

    Eigen::ArrayXXf output(nSamples, c.nChannels);
    Eigen::ArrayXXf output2(nSamples, c.nChannels);
    filter.process({input, cutoff.transpose().replicate(nSamples,1), gain.transpose().replicate(nSamples,1), resonance.transpose().replicate(nSamples,1),}, output);
    filter2nd.process(input, output2);

    float error = (output - output2).abs2().sum() / output.abs2().sum();
    fmt::print("Error: {}\n", error);
    EXPECT_LT(error, 1e-10f);
}