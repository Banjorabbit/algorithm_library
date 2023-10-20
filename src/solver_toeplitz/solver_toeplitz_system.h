#pragma once
#include "framework/framework.h"
#include "algorithm_library/solver_toeplitz.h"

// Toeplitz Solver that can solve multiple right hand sides at once.
//
// Input:
// aToeplitz - 1D complex array that corresponds to the first row in lefthand toeplitz matrix
// BRighthand - 2D complex array where each column is a right hand side vector
//
// author: Kristian Timm Andersen

// ----------------------------- Cubic interpolation ------------------------------------------------------

class SolverToeplitzSystem : public IAlgorithm<SolverToeplitzConfiguration, SolverToeplitzSystem>
{
public:
	SolverToeplitzSystem(Coefficients c = Coefficients()) :
		IAlgorithm<SolverToeplitzConfiguration, SolverToeplitzSystem>{ c }
	{ }
	
	inline void processOn(Input x, Output y)
	{
		const auto nFilt = x.BRighthand.rows();
		const auto nChan = x.BRighthand.cols();

		Eigen::ArrayXcf c1(nFilt - 1);
		Eigen::ArrayXcf c2(nFilt - 1);
		std::complex<float> r1 = x.aToeplitz(0), r2 = { 0,0 }, r3 = { 0,0 }, r5 = { 0,0 }, r6 = { 0,0 };
		r1 += 1e-10f; // add small regularization
		std::complex<float> r1Norm = std::conj(r1) / std::norm(r1);

		y.row(0) = x.BRighthand.row(0) * r1Norm;

		r5 = std::conj(x.aToeplitz(1));
		r2 = -r5 * r1Norm;
		r3 = -x.aToeplitz(1) * r1Norm;
		r1 += r5 * r3;
		r1Norm = std::conj(r1) / std::norm(r1);
		c2(0) = r3;

		y.row(1) = (x.BRighthand.row(1) - std::conj(x.aToeplitz(1)) * y.row(0)) * r1Norm;
		y.row(0) += c2(0) * y.row(1);

		for (int isub = 2; isub < nFilt; isub++) {
			r5 = std::conj(x.aToeplitz(isub));
			r6 = x.aToeplitz(isub);
			c1(isub - 1) = r2;
			for (int i = 0; i <= isub - 2; i++) {
				r5 += std::conj(x.aToeplitz(i + 1)) * c1(isub - (i + 1));
				r6 += x.aToeplitz(i + 1) * c2(i);
			}
			r2 = -r5 * r1Norm;
			r3 = -r6 * r1Norm;
			r1 += r5 * r3;
			r1Norm = std::conj(r1) / std::norm(r1);

			r6 = c2(0);
			c2(isub - 1) = 0;
			for (int i = 1; i <= isub - 1; i++) {

				r5 = c2(i);
				c2(i) = c1(i) * r3 + r6;
				c1(i) += r6 * r2;
				r6 = r5;
			}
			c2(0) = r3;

			for (int ichan = 0; ichan < nChan; ichan++) {
				r5 = 0.f;
				for (int i = 0; i <= isub - 1; i++) {
					r5 += std::conj(x.aToeplitz(i + 1)) * y(isub - (i + 1), ichan);
				}
				r6 = (x.BRighthand(isub, ichan) - r5) * r1Norm;

				for (int i = 0; i <= isub - 1; i++) {
					y(i, ichan) += c2(i) * r6;
				}
				y(isub, ichan) = r6;
			}
		}
	}
};
