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

struct SolverToeplitzInput
{
	I::Complex aToeplitz;
	I::Complex2D BRighthand;
};

struct SolverToeplitzConfiguration : public Configuration<SolverToeplitzInput, O::Complex2D>
{
	static auto validateInput(Input input, const Coefficients& c) { return (input.aToeplitz.size() > 0) && (input.aToeplitz.size() == input.BRighthand.rows()) && (input.BRighthand.rows() == input.BRighthand.cols()); }
	static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXcf(input.BRighthand.rows(), input.BRighthand.cols()); }

	template<typename Talgo>
	struct Test
	{
		Talgo algo;
		Eigen::ArrayXcf aToeplitz;
		Eigen::ArrayXXcf BRighthand;
		Eigen::ArrayXXcf output;

		Test(const Coefficients& c = {})
		{
			aToeplitz.resize(8);
			aToeplitz.setRandom();
			aToeplitz(0) = 1;
			BRighthand.resize(8, 8);
			BRighthand.setRandom();
			output = initOutput({aToeplitz, BRighthand}, c);
		}

		inline void processAlgorithm() { algo.process({ aToeplitz, BRighthand }, output); }
		bool isTestOutputFinite() const { return output.allFinite(); }
	};
};

class SolverToeplitz : public Algorithm<SolverToeplitzConfiguration>
{ };
