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
	struct Parameters
	{
		float sFactor = 0.f;
		DEFINE_TUNABLE_PARAMETERS(sFactor)
	};

	template<typename Talgo>
	struct Test
	{
		Eigen::ArrayXXf xGiven;
		Eigen::ArrayXXf yGiven;
		Eigen::ArrayXXf xDesired;
		Eigen::ArrayXXf yDesired;

		Test()
		{
			int n = 10;
			int nOS = 100;
			int nChannels = 2;
			xGiven.resize(n, nChannels);
			yGiven.resize(n, nChannels);
			xDesired.resize(nOS, nChannels);
			yDesired.resize(nOS, nChannels);
			xGiven = Eigen::ArrayXf::LinSpaced(n, 0, n - 1).replicate(1, nChannels);
			yGiven.setRandom();
			xDesired = Eigen::ArrayXf::LinSpaced(nOS, 0, n - 1).replicate(1, nChannels);
		}

		inline void processAlgorithm(Talgo& algo) { algo.process({ xGiven, yGiven, xDesired }, yDesired); }
		bool isTestOutputFinite() const { return yDesired.allFinite(); }
	};
};

class Spline : public Algorithm<SplineConfiguration>
{ 
public:
	Spline() = default;
	Spline(const Parameters& p);
};