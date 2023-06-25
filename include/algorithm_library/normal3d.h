#pragma once
#include "interface/interface.h"


// Calculate the 3-dimensional normal vector of a plane for each point in the plane. 
// The input is a 2-dimensional matrix containing the z-value of each point in a x/y grid
// The output is a 2-dimensional matrix, where the first dimension is 3 times the size of the input to hold the interleaved 3-dimensional normal vector.
//
// author: Kristian Timm Andersen

// --------------------------------------------- Filterbank Analysis ----------------------------------------------------------------

struct Normal3dConfiguration : public Configuration<I::Real2D, O::Real2D>
{
	struct Coefficients
	{
        int nValuesX = 257;
		DEFINE_TUNABLE_COEFFICIENTS(nValuesX)
	};

	template<typename Talgo>
	struct Test
	{
		Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;
        int nValuesY = 10;

		Test()
		{
			Talgo algo;
			auto c = algo.getCoefficients();
			input.resize(c.nValuesX, nValuesY);
            output.resize(3 * c.nValuesX, nValuesY);
			input.setRandom();
		}
	
		inline void processAlgorithm(Talgo& algo) { algo.process(input, output); }
		bool isTestOutputFinite() const { return output.allFinite(); }
	};
};

class Normal3d : public Algorithm<Normal3dConfiguration>
{
public:
	Normal3d() = default;
	Normal3d(const Coefficients& c);
};
