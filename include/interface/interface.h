#pragma once
#define EIGEN_DENSEBASE_PLUGIN "interface/get_dynamic_memory_size.h" //  member function added to Eigen DenseBase class to get dynamic memory size of array and matrices
#define EIGEN_MPL2_ONLY // don't allow LGPL licensed code from Eigen
#include <Eigen/Dense> // Eigen Library. 
#include <Eigen/IterativeLinearSolvers>
#include "public_algorithm.h" // Base Algorithm class
#include "configuration.h" // Configuration class
#include "input_output.h" // Input/Output structs. 
#include "macros_json.h" // macros for coefficients and parameters
#include "constrained_type.h" // ConstrainedType class

// This is the main interface file that includes the necessary files for the public interface
//
// author: Kristian Timm Andersen, 2019