#include "solver_toeplitz/solver_toeplitz_system.h"

template<> 
Algorithm<SolverToeplitzConfiguration>::Algorithm() 
{
    pimpl = std::make_unique<Impl<SolverToeplitzSystem, SolverToeplitzConfiguration>>();
} 

template<> 
Algorithm<SolverToeplitzConfiguration>::~Algorithm() {} 

template<>
Algorithm<SolverToeplitzConfiguration>::Algorithm(const Coefficients& c) 
{
    pimpl = std::make_unique<Impl<SolverToeplitzSystem, SolverToeplitzConfiguration>>(c);
} 

SolverToeplitz::SolverToeplitz(const Coefficients& c) : Algorithm<SolverToeplitzConfiguration>{c} {}