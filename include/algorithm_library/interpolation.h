#pragma once
#include "interface/interface.h"

// Interpolation method.
// Input is a size 4 array and FractionalDelay is
// in the interval[0, 1[ between Input[1] and Input[2].
//
// author: Kristian Timm Andersen

// -----------------------------  interpolation of one sample --------------------------------------------

struct InterpolationSampleConfiguration
{
    struct Input
    {
        I::Real4 samples;
        I::Float fractionalDelay; // fractional index to interpolate. It must be between 0.0 - 1.0
    };

    using Output = O::Float;

    struct Coefficients { DEFINE_NO_TUNABLE_COEFFICIENTS };

    struct Parameters { DEFINE_NO_TUNABLE_PARAMETERS };

    static auto validInput(Input input, const Coefficients& c) { return (input.samples.size() == 4) && (input.fractionalDelay >= 0.f) && (input.fractionalDelay <= 1.f);	}
    static auto initOutput(Input input, const Coefficients& c) { return 0.f; }

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        Eigen::Array4f samples;
        float fractionalIndex; // index must be between 0.0 - 1.0
        float output; // output has same size as fractionalIndices

        Example() : Example(Coefficients()) {}
        Example(const Coefficients& c)
        {
            samples.setRandom();
            fractionalIndex = 0.3f;
            output = 0.f;
        }

        void processAlgorithm() { algo.process({ samples, fractionalIndex }, output); }
        bool isExampleOutputValid() const { return std::isfinite(output); }
    };
};

class InterpolationSample : public Algorithm<InterpolationSampleConfiguration>
{
public:
    InterpolationSample() = default;
    InterpolationSample(const Coefficients& c);
};


// -----------------------------  interpolation of array ------------------------------------------------------

struct InterpolationConfiguration
{
    struct Input
    {
        I::Real samples;
        I::Real fractionalIndices; // fractional indices to interpolate. They must be in increasing order. Since 4 points are needed to do cubic interpolation it is not possible to interpolate indices smaller than 1.0 and larger than samples.size()-2.0
    };

    using Output = O::Real;

    struct Coefficients { DEFINE_NO_TUNABLE_COEFFICIENTS };

    struct Parameters { DEFINE_NO_TUNABLE_PARAMETERS };

    static auto validInput(Input input, const Coefficients& c) { return (input.samples.size() >= 4) && (input.fractionalIndices >= 1.0).all() && (input.fractionalIndices <= (input.samples.size() - 2.f)).all();	}
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXf(input.fractionalIndices.size()); }

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        Eigen::ArrayXf samples;
        Eigen::ArrayXf fractionalIndices; // indices must be between 1.0 and samples.size()-2.0 and in non-decreasing order
        Eigen::ArrayXf output; // output has same size as fractionalIndices

        Example(const Coefficients& c = {})
        {
            const int N = 256;
            samples = Eigen::ArrayXf::Random(N);
            fractionalIndices = Eigen::ArrayXf::Random(N).abs() * (N - 3.f) + 1.f; // indices must be between 1.0 and N-2.0
            std::sort(&fractionalIndices(0), &fractionalIndices(0) + N - 1); // indices must be in increasing order
            output = initOutput({samples, fractionalIndices}, c);
        }

        inline void processAlgorithm() { algo.process({ samples, fractionalIndices }, output); }
        bool isExampleOutputValid() const { return output.isFinite().all(); }
    };
};

class Interpolation : public Algorithm<InterpolationConfiguration>
{
public:
    Interpolation() = default;
    Interpolation(const Coefficients& c);
};

// -----------------------------  interpolation of array with constant fractionalDelay. This is faster ------------------------------------------------------

struct InterpolationConstantConfiguration
{
    using Input = I::Real;
    using Output = O::Real;

    struct Coefficients
    {
        float fractionalDelay = 0.5;
        DEFINE_TUNABLE_COEFFICIENTS(fractionalDelay)
    };

    struct Parameters { DEFINE_NO_TUNABLE_PARAMETERS };

    static auto validInput(Input input, const Coefficients& c) { return (input.size() >= 4); }
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXf(input.size() - 3); }

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        Eigen::ArrayXf samples;
        Eigen::ArrayXf output;
        int n;

        Example() : Example(Coefficients()) {}
        Example(const Coefficients& c) : algo(c)
        {
            n = 256;
            samples = Eigen::ArrayXf::Random(n);
            output = initOutput(samples, c); // interpolation needs 4 values, so there will be 3 less outputs than inputs
        }

        inline void processAlgorithm() { algo.process(samples, output); }
        bool isExampleOutputValid() const { return output.allFinite() && (output.rows() == n-3); }
    };
};

class InterpolationConstant : public Algorithm<InterpolationConstantConfiguration>
{
public:
    InterpolationConstant() = default;
    InterpolationConstant(const Coefficients& c);
};

