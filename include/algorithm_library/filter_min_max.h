
#pragma once
#include "interface/interface.h"

// StreamingMinMax finds the minimum and maximum value over a
// window with length C.Length for each new sample. It requires on
// average no more than 3 comparisons per sample. The algorithm uses 2
// double-ended queues for the minimum and maximum indices. A delay line
// is also used internally since in a true streaming application you need 
// to be able to call the algorithm succesively with new frames (or just 1 new sample),
// and you are not guaranteed that the input frame is as long as C.Length. 
// To be able to preallocate, the queues have been implemented as circular buffers. 
//
// A symmetric version of StreamingMinMax has been implemented below called "Filter". 
// It might be necessary to call the public ResetInitialValues function before Process, 
// if certain initial conditions are required. Also versions that only find Min/Max have been implemented.
//
// ref: Daniel Lemire, STREAMING MAXIMUM - MINIMUM FILTER USING NO MORE THAN THREE COMPARISONS PER ELEMENT
//
// author : Kristian Timm Andersen

struct FilterMinMaxOutput
{
	O::Real2D minValue;
	O::Real2D maxValue;
};

struct BaseFilterMinMaxConfiguration  : public Configuration<I::Real2D, FilterMinMaxOutput>
{
    struct Coefficients
	{
		int filterLength = 100;
		int nChannels = 2;
		DEFINE_TUNABLE_COEFFICIENTS(filterLength, nChannels)
	};

    static auto validateInput(Input input, const Coefficients& c) { return (input.rows() > 0) && (input.cols() == c.nChannels);}
    static auto initOutput(Input input, const Coefficients& c) { return std::make_tuple( Eigen::ArrayXXf(input.rows(), c.nChannels), Eigen::ArrayXXf(input.rows(), c.nChannels) ); }
    
    template<typename Talgo>
	struct Test
	{
        Talgo algo;
		Eigen::ArrayXXf minValue;
		Eigen::ArrayXXf maxValue;
		Eigen::ArrayXXf input;

        Test(const Coefficients& c = {}) : algo(c)
		{
            const int samples = 1000;
			input.resize(samples, c.nChannels);
			input.setRandom();
            std::tie(minValue, maxValue) = initOutput(input, c);
		}

		inline void processAlgorithm() { algo.process(input, { minValue, maxValue }); }
		bool isTestOutputFinite() const { return minValue.allFinite() && maxValue.allFinite(); }

	};
};

struct FilterMinMaxConfiguration : public BaseFilterMinMaxConfiguration {};
struct StreamingMinMaxConfiguration : public BaseFilterMinMaxConfiguration {};

class StreamingMinMax : public Algorithm<StreamingMinMaxConfiguration> 
{
public:
    StreamingMinMax() = default;
    StreamingMinMax(const Coefficients& c);

    // It might be necessary to call resetInitialValues function before process if certain initial conditions are required.
	void resetInitialValue(const float inputOld); 
	void resetInitialValue(const I::Real inputOld); 
};

class FilterMinMax : public Algorithm<FilterMinMaxConfiguration> 
{
public:
    FilterMinMax() = default;
    FilterMinMax(const Coefficients& c);

    // It might be necessary to call resetInitialValues function before process if certain initial conditions are required.
	void resetInitialValue(const float inputOld); 
	void resetInitialValue(const I::Real inputOld); 
};
