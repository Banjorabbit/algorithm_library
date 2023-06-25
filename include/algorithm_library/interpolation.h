#pragma once
#include "interface/interface.h"

// Interpolation method.
// Input is a size 4 array and FractionalDelay is
// in the interval[0, 1[ between Input[1] and Input[2].
//
// author: Kristian Timm Andersen

// -----------------------------  interpolation of one sample --------------------------------------------

struct InterpolationSampleInput
{
	I::Real4 samples;
	I::Float fractionalDelay; // fractional index to interpolate. It must be between 0.0 - 1.0
};

struct InterpolationSampleConfiguration : public Configuration<InterpolationSampleInput, O::Float>
{
	template<typename Talgo>
	struct Test
	{
		Eigen::Array4f samples;
		float fractionalIndex; // index must be between 0.0 - 1.0
		float output; // output has same size as fractionalIndices

		Test()
		{
			samples.setRandom();
			fractionalIndex = 0.3f;
		}

		void processAlgorithm(Talgo& algo) { algo.process({ samples, fractionalIndex }, output); }
		bool isTestOutputFinite() const { return std::isfinite(output); }
	};
};

class InterpolationSample : public Algorithm<InterpolationSampleConfiguration>
{
public:
	InterpolationSample() = default;
};


// -----------------------------  interpolation of array ------------------------------------------------------

struct InterpolationInput
{
	I::Real samples;
	I::Real fractionalIndices; // fractional indices to interpolate. They must be in increasing order. Since 4 points are needed to do cubic interpolation it is not possible to interpolate indices smaller than 1.0 and larger than samples.size()-2.0
};

struct InterpolationConfiguration : public Configuration<InterpolationInput, O::Real>
{

	template<typename Talgo>
	struct Test
	{
		Eigen::ArrayXf samples;
		Eigen::ArrayXf fractionalIndices; // indices must be between 1.0 and samples.size()-2.0 and in non-decreasing order
		Eigen::ArrayXf output; // output has same size as fractionalIndices

		Test()
		{
			const int N = 256;
			samples.resize(N);
			samples.setRandom();
			fractionalIndices.resize(N);
			fractionalIndices = Eigen::ArrayXf::Random(N).abs() * (N - 3.f) + 1.f; // indices must be between 1.0 and N-2.0
			std::sort(&fractionalIndices(0), &fractionalIndices(0) + N - 1); // indices must be in increasing order
			output.resize(N);
		}

		inline void processAlgorithm(Talgo& algo) { algo.process({ samples, fractionalIndices }, output); }
		bool isTestOutputFinite() const { return output.isFinite().all(); }
	};
};

class Interpolation : public Algorithm<InterpolationConfiguration>
{
public:
	Interpolation() = default;
};

// -----------------------------  interpolation of array with constant fractionalDelay. This is faster ------------------------------------------------------

struct InterpolationConstantConfiguration : public Configuration<I::Real, O::Real>
{
	struct Coefficients
	{
		float fractionalDelay = 0.5;
		DEFINE_TUNABLE_COEFFICIENTS(fractionalDelay)
	};

	template<typename Talgo>
	struct Test
	{
		Eigen::ArrayXf samples;
		Eigen::ArrayXf output;

		Test()
		{
			const int N = 256;
			samples.resize(N);
			samples.setRandom();
			output.setRandom(N - 3); // interpolation needs 4 values, so there will be 3 less outputs than inputs
		}

		inline void processAlgorithm(Talgo& algo) { algo.process(samples, output); }
		bool isTestOutputFinite() const { return output.allFinite(); }
	};
};

class InterpolationConstant : public Algorithm<InterpolationConstantConfiguration>
{
public:
	InterpolationConstant() = default;
	InterpolationConstant(const Coefficients& c);
};

