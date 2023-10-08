#pragma once
#include "interface/interface.h"

// Spline interpolates the set of data points xGiven/yGiven evaluted at xDesired.
// The inputs must be ordered in increasing x order or the algorithm will give nonsense output.
//
// author: Kristian Timm Andersen

struct SplineInput
{
	I::Real2D xGiven;
	I::Real2D yGiven;
	I::Real2D xDesired;
};

struct SplineConfiguration : public Configuration<SplineInput, O::Real2D>
{
	struct Coefficients
	{
		int nChannels = 2;
		DEFINE_TUNABLE_COEFFICIENTS(nChannels)
	};
	struct Parameters
	{
		float sFactor = 0.f;
		DEFINE_TUNABLE_PARAMETERS(sFactor)
	};

	template<typename Talgo>
	struct Test
	{
		Talgo algo;
		Eigen::ArrayXXf xGiven;
		Eigen::ArrayXXf yGiven;
		Eigen::ArrayXXf xDesired;
		Eigen::ArrayXXf yDesired;

		Test(const Coefficients& c = {}) : algo(c)
		{
			int n = 10; // number of input samples per channel
			int nOS = 100; // number of output samples per channel
			xGiven = Eigen::ArrayXf::LinSpaced(n, 0, n - 1).replicate(1, c.nChannels);
			yGiven.resize(n, c.nChannels);
			yGiven.setRandom();
			xDesired = Eigen::ArrayXf::LinSpaced(nOS, 0, n - 1).replicate(1, c.nChannels);
			yDesired.resize(nOS, c.nChannels);
		}

		inline void processAlgorithm() { algo.process({ xGiven, yGiven, xDesired }, yDesired); }
		bool isTestOutputFinite() const { return yDesired.allFinite(); }
	};
};

class Spline : public Algorithm<SplineConfiguration>
{ 
public:
	Spline() = default;
	Spline(const Setup& s);
	Spline(const Coefficients& c);
};