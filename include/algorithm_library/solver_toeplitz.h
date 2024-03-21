#pragma once
#include "interface/interface.h"

// Toeplitz Solver that can solve multiple right hand sides at once.
//
// Input:
// aToeplitz - 1D complex array that corresponds to the first row in lefthand toeplitz matrix
// BRighthand - 2D complex array where each column is a right hand side vector
//
// author: Kristian Timm Andersen

// ----------------------------- Cubic interpolation ------------------------------------------------------

struct SolverToeplitzConfiguration
{
    struct Input
    {
        I::Complex aToeplitz;
        I::Complex2D BRighthand;
    };

    using Output = O::Complex2D;

    struct Coefficients { 
        int nRHS = 8; // number of right-hand sides to solve (columns in BRighthand)
        DEFINE_TUNABLE_COEFFICIENTS(nRHS)
    };

    struct Parameters { DEFINE_NO_TUNABLE_PARAMETERS };

    static auto validInput(Input input, const Coefficients& c) { return (input.aToeplitz.size() > 0) && (input.aToeplitz.size() == input.BRighthand.rows()) && (input.BRighthand.cols() == c.nRHS); }
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXcf(input.BRighthand.rows(), c.nRHS); }

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        Eigen::ArrayXcf aToeplitz;
        Eigen::ArrayXXcf BRighthand;
        Eigen::ArrayXXcf output;
        int nRHS;

        Example() : Example(Coefficients()) {}
        Example(const Coefficients& c)
        {
            nRHS = c.nRHS;
            aToeplitz.resize(8);
            aToeplitz.setRandom();
            aToeplitz(0) = 1;
            BRighthand.resize(8, nRHS);
            BRighthand.setRandom();
            output = initOutput({aToeplitz, BRighthand}, c);
        }

        inline void processAlgorithm() { algo.process({ aToeplitz, BRighthand }, output); }
        bool isExampleOutputValid() const { return output.allFinite() && (output.rows() == 8) && (output.cols() == nRHS); }
    };
};

class SolverToeplitz : public Algorithm<SolverToeplitzConfiguration>
{
public:
    SolverToeplitz() = default;
    SolverToeplitz(const Coefficients& c);
 };
