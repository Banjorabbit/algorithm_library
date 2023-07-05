#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "min_phase_spectrum/min_phase_spectrum_cepstrum.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(MinPhaseSpectrumCepstrum, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<MinPhaseSpectrumCepstrum>());
}