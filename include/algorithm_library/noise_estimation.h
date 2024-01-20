#pragma once
#include "interface/interface.h"

// noise estimation based on an activity detector. The more activity in the power spectra, the more the output is smoothed.
//
// author: Kristian Timm Andersen
struct NoiseEstimationConfiguration
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
        float smoothingTConstant = .72f;
        DEFINE_TUNABLE_PARAMETERS(smoothingTConstant)
    };

    static auto validInput(Input input, const Coefficients& c) 
    { 
        return (input.rows() == c.nBands) && (input.cols() == c.nChannels);
    }

    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf(c.nBands, c.nChannels); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;
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
        bool isTestOutputValid() const 
        { 
            bool test = output.allFinite();
            test &= (output >= 0).all();
            test &= (output.rows() == nBands) && (output.cols() == nChannels);
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