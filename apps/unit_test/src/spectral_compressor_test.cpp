#include "spectral_compressor/spectral_compressor_wola.h"
#include "spectral_compressor/spectral_selector.h"
#include "unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(SpectralCompressor, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<SpectralCompressorWOLA>()); }

TEST(SpectralSelector, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<SpectralSelector>()); }
