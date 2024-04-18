#include "iir_filter_non_parametric/iir_filter_design_non_parametric.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(IIRFilterNonParametric, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<IIRFilterTDFNonParametric>()); }
