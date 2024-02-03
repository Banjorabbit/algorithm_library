#pragma once
#include "interface/interface.h"

// Acticity detection calculated in the frequency domain based on noise estimation and activity detector. 
//
// author: Kristian Timm Andersen

struct ActivityDetectionConfiguration
{
    using Input = I::Real2D;
    using Output = O::Real2D;

    struct Coefficients
    {
        float filterbankRate = 125.f;
        int nBands = 257;
        int nChannels = 2;
        DEFINE_TUNABLE_COEFFICIENTS(filterbankRate, nChannels, nBands)
    };

    struct Parameters
    {
        float smoothingTConstant = .072f; // how much to smooth the noise estimation
        DEFINE_TUNABLE_PARAMETERS(smoothingTConstant)
    };

    static auto validInput(Input input, const Coefficients& c) 
    { 
        return (input >= 0).all() && (input.rows() == c.nBands) && (input.cols() == c.nChannels);
    }

    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf(c.nBands, c.nChannels); }

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;
        int nBands, nChannels;

        Example() : Example(Coefficients()) {}
        Example(const Coefficients& c) : algo(c)
        {
            nChannels = c.nChannels;
            nBands = c.nBands;
            input = Eigen::ArrayXXf::Random(nBands, nChannels).abs2();
            output = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, output); }
        bool isExampleOutputValid() const { return (output>=0).all() && (output.rows() == nBands) && (output.cols() == nChannels); }
    };
};

class ActivityDetection : public Algorithm<ActivityDetectionConfiguration>
{
public:
    ActivityDetection() = default;
    ActivityDetection(const Coefficients& c);
};

// ActivityDetectionFused is a version of ActivityDetection where the output is combined into a single boolean value
struct ActivityDetectionFusedConfiguration
{
    using Input = I::Real2D;
    using Output = O::Boolean;

    struct Coefficients
    {
        float filterbankRate = 125.f;
        int nBands = 257;
        int nChannels = 2;
        DEFINE_TUNABLE_COEFFICIENTS(filterbankRate, nChannels, nBands)
    };

    struct Parameters
    {
		float activityThreshold = 1.f; // parameter to set how aggresive the detection is
		DEFINE_TUNABLE_PARAMETERS(activityThreshold)
    };

    static auto validInput(Input input, const Coefficients& c) 
    { 
        return (input >= 0).all() && (input.rows() == c.nBands) && (input.cols() == c.nChannels);
    }

    static auto initOutput(Input input, const Coefficients& c) { return bool(); }

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        bool output;
        int nBands, nChannels;

        Example() : Example(Coefficients()) {}
        Example(const Coefficients& c) : algo(c)
        {
            nChannels = c.nChannels;
            nBands = c.nBands;
            input = Eigen::ArrayXXf::Random(nBands, nChannels).abs2();
            output = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, output); }
        bool isExampleOutputValid() const { return true; } // output is a bool so it is always valid
    };
};

class ActivityDetectionFused : public Algorithm<ActivityDetectionFusedConfiguration>
{
public:
    ActivityDetectionFused() = default;
    ActivityDetectionFused(const Coefficients& c);
};
