#include "delay/circular_buffer.h"
#include "framework/unit_test.h"
#include "gtest/gtest.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(CircularBuffer, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<CircularBuffer>()); }

TEST(CircularBufferSingleChannel, Interface) { EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<CircularBufferSingleChannel>()); }
