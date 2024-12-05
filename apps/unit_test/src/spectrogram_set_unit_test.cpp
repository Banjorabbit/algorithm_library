#include "spectrogram/spectrogram_set.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(SpectrogramSet, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<SpectrogramSet>()); }