#pragma once
#include "interface/interface.h"

// noise estimation based on an activity detector. The more activity in the power spectra, the more the output is smoothed.
//
// author: Kristian Timm Andersen
struct NoiseEstimationConfiguration
{
    using Input = I::Real2D;
    struct Output
    {
        O::Real2D powerNoise;
        O::Real2D activity;
    };

    struct Coefficients
    {
        float filterbankRate = 125.f;
        int nBands = 257;
        int nChannels = 2;
        DEFINE_TUNABLE_COEFFICIENTS(filterbankRate, nChannels, nBands)
    };

    struct Parameters
    {
        float smoothingTConstant = .072f;
        DEFINE_TUNABLE_PARAMETERS(smoothingTConstant)
    };

    static auto validInput(Input input, const Coefficients& c) 
    { 
        return (input >= 0.f).all() && (input.rows() == c.nBands) && (input.cols() == c.nChannels);
    }

    static auto initOutput(Input input, const Coefficients& c) { return std::make_tuple(Eigen::ArrayXXf(c.nBands, c.nChannels), Eigen::ArrayXXf(c.nBands, c.nChannels)); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;
        Eigen::ArrayXXf activity;
        int nBands, nChannels;

        Test() : Test(Coefficients()) {}
        Test(const Coefficients& c) : algo(c)
        {
            nChannels = c.nChannels;
            nBands = c.nBands;
            input = Eigen::ArrayXXf::Random(nBands, nChannels).abs2();
            std::tie(output, activity) = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, {output, activity}); }
        bool isTestOutputValid() const 
        { 
            bool test = output.allFinite() && activity.allFinite();
            test &= (output >= 0).all() && (activity >= 0).all();
            test &= (output.rows() == nBands) && (output.cols() == nChannels) && (activity.rows() == nBands) && (activity.cols() == nChannels);
            return test;
        }
    };
};

class NoiseEstimation : public Algorithm<NoiseEstimationConfiguration>
{
public:
    NoiseEstimation() = default;
    NoiseEstimation(const Coefficients& c);
};