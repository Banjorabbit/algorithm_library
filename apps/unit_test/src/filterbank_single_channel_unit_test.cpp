#include "filterbank/filterbank_single_channel.h"
#include "unit_test.h"
#include "gtest/gtest.h"

TEST(FilterbankSingleChannel, InterfaceAnalysisSingleChannel) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<FilterbankAnalysisSingleChannel>()); }

TEST(FilterbankSingleChannel, InterfaceSynthesisSingleChannel) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<FilterbankSynthesisSingleChannel>()); }
