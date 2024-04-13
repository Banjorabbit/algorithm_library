#include "gtest/gtest.h"
#include "framework/unit_test.h"
#include "solver_toeplitz/solver_toeplitz_system.h"

using namespace Eigen;

// --------------------------------------------- TEST CASES ---------------------------------------------

TEST(SolverToeplitzSystem, Interface)
{
	EXPECT_TRUE(InterfaceTests::algorithmInterfaceTest<SolverToeplitzSystem>());
}

TEST(SolverToeplitzSystem, CalculationTest)
{
	using namespace std::literals;

	SolverToeplitzSystem toeplitzSolver;
	ArrayXcf aToeplitz(2);
	ArrayXXcf bRightHand(2, 3);
	aToeplitz << 0.4882f - 0.1961if, -0.1774f + 1.4193if;
	bRightHand << -0.2103f + 0.2486if, 0.6536f - 0.0990if, -0.7586f + 0.6976if, -0.3118f - 0.2045if, -0.7980f - 0.8078if, -0.3473f - 0.6120if;
	ArrayXXcf output(2, 3);
	ArrayXXcf outputRef(2, 3);
	outputRef << 0.1825f - 0.3271if, 0.4846f - 0.4906if, 0.5752f - 0.5329if, 0.3284f + 0.1247if, 0.1188f - 0.3764if, 0.8237f + 0.5558if;
	toeplitzSolver.process({ aToeplitz, bRightHand }, output);
	float error = (output - outputRef).abs2().sum();
	fmt::print("Error: {}\n", error);
	EXPECT_LT(error, 1e-7f);
}