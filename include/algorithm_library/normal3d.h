#pragma once
#include "interface/interface.h"


// Calculate the 3-dimensional normal vector of a plane for each point in the plane. 
// The input is a 2-dimensional matrix containing the z-value of each point in a x/y grid
// The output is a 2-dimensional matrix, where the 1st output dimension is 3 times the size of the 1st input dimension to hold the interleaved 3-dimensional normal vector.
//
// author: Kristian Timm Andersen

// --------------------------------------------- Filterbank Analysis ----------------------------------------------------------------

struct Normal3dConfiguration : public Configuration<I::Real2D, O::Real2D>
{
	struct Coefficients
	{
        int nValuesX = 257;
		int nChannels = 2;
		DEFINE_TUNABLE_COEFFICIENTS(nValuesX, nChannels)
	};

	struct Parameters
	{
		float distance1 = 31.25f; // distance between values along 1st dimension
		float distance2 = 8.f; // distance between values along 2nd dimension
		DEFINE_TUNABLE_PARAMETERS(distance1, distance2)
	};

	template<typename Talgo>
	struct Test
	{
		Talgo algo;
		Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;
        int nValuesY = 10;

		Test(const Coefficients& c = {}) : algo(c)
		{
			input.resize(c.nValuesX, nValuesY);
            output.resize(3 * c.nValuesX, nValuesY);
			input.setRandom();
		}
	
		inline void processAlgorithm() { algo.process(input, output); }
		bool isTestOutputFinite() const { return output.allFinite(); }
	};
};

class Normal3d : public Algorithm<Normal3dConfiguration>
{
public:
	Normal3d() = default;
	Normal3d(const Coefficients& c);
	Normal3d(const Setup& s);
};
