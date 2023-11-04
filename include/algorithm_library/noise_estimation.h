#pragma once
#include "interface/interface.h"

// noise estimation based on an activity detector. The more activity in the power spectra, the more the output is smoothed.
//
// author: Kristian Timm Andersen
struct NoiseEstimationConfiguration : public Configuration<I::Real2D, O::Real2D>
{
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

        Test() : Test(Coefficients()) {}
        Test(const Coefficients& c) : algo(c)
        {
            input = Eigen::ArrayXXf(c.nBands, c.nChannels);
            input.setRandom();
            output = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, output); }
        bool isTestOutputFinite() const { return output.allFinite(); }
    };
};

class NoiseEstimation : public Algorithm<NoiseEstimationConfiguration>
{
public:
    NoiseEstimation() = default;
    NoiseEstimation(const Coefficients& c);
};