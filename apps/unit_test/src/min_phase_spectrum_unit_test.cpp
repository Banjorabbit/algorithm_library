#include "min_phase_spectrum/min_phase_spectrum_cepstrum.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(MinPhaseSpectrumCepstrum, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<MinPhaseSpectrumCepstrum>()); }