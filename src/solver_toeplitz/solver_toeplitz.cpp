#include "solver_toeplitz/solver_toeplitz_system.h"

DEFINE_SOURCE_INTERFACE(SolverToeplitzConfiguration, SolverToeplitzSystem)

SolverToeplitz::SolverToeplitz(const Coefficients& c) : Algorithm<SolverToeplitzConfiguration>{c} {}