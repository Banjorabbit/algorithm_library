#pragma once
#include "interface/interface.h"

// Acticity detection calculated in the frequency domain based on noise estimation and activity detector. 
//
// author: Kristian Timm Andersen

struct ActivityDetectionFrequencyDomainConfiguration
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
    struct Test
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        bool output;
        int nBands, nChannels;

        Test() : Test(Coefficients()) {}
        Test(const Coefficients& c) : algo(c)
        {
            nChannels = c.nChannels;
            nBands = c.nBands;
            input = Eigen::ArrayXXf::Random(nBands, nChannels).abs2();
            output = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, output); }
        bool isTestOutputValid() const { return true; } // output is a bool so it is always valid
    };
};

class ActivityDetectionFrequencyDomain : public Algorithm<ActivityDetectionFrequencyDomainConfiguration>
{
public:
    ActivityDetectionFrequencyDomain() = default;
    ActivityDetectionFrequencyDomain(const Coefficients& c);
};
