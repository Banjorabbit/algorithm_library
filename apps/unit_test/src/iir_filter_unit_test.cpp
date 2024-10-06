#include "iir_filter/iir_filter_2nd_order.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(IIRFilter, Interface2ndOrder) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<IIRFilter2ndOrder>()); }

TEST(IIRFilter, InterfaceCascaded) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<IIRFilterCascaded>()); }

// test the valid input/output methods for the public class IIRFilter
TEST(IIRFilter, publicValidinputOutput)
{
    IIRFilter filter;
    auto input = filter.initInput();
    auto output = filter.initOutput(input);
    filter.process(input, output);

    EXPECT_TRUE(filter.validInput(input));
    EXPECT_TRUE(filter.validOutput(output));
}