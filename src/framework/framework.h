#pragma once
#define EIGEN_DENSEBASE_PLUGIN "algorithm_library/interface/get_dynamic_memory_size.h" //  member function added to Eigen DenseBase class to get dynamic memory size of array and matrices
#define EIGEN_MPL2_ONLY // don't allow LGPL licensed code from Eigen
#include "Eigen/Dense" // Eigen Library. 
#include "Eigen/IterativeLinearSolvers"
#include "base_algorithm.h" // Base and Algorithm class
#include "vector_algo.h" // VectorAlgo class
#include "macros.h" // macros for member algorithms
#include "algorithm_library/interface/interface.h"

// This is the main header file that includes the necessary files for developing algorithms using the AlgorithmFramework
//
// author: Kristian Timm Andersen, 2019