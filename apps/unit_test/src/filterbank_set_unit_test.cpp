#include "filterbank_set/filterbank_set_wola.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(FilterbankSetAnalysis, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<FilterbankSetAnalysisWOLA>()); }
