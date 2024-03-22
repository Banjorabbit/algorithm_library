#pragma once
#include "interface/interface.h"

// Gain Calculation given the aposteriori SNR as input: P_{s+n}/Phat_{n}
//
// author: Kristian Timm Andersen

struct GainCalculationConfiguration
{
    using Input = I::Real2D;
    using Output = O::Real2D;

    struct Coefficients
    {
        int nChannels = 2;
        float filterbankRate = 125.f;
        int nBands = 257;
        DEFINE_TUNABLE_COEFFICIENTS(nChannels, filterbankRate, nBands)
    };

    struct Parameters
    {
        float minimumGaindB = -15.f;
        float exponential = 3.f;
        float upTConstant = 0.003f;
        float downTConstant = 0.01f;
        DEFINE_TUNABLE_PARAMETERS(minimumGaindB, exponential, upTConstant, downTConstant)
    };

    static auto validInput(Input input, const Coefficients& c) 
    { 
        return (input >= 0.f).all() && (input.rows() == c.nBands) && (input.cols() == c.nChannels);
    }

    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf(c.nBands, c.nChannels); }

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        Eigen::ArrayXXf snrAposteriori;
        Eigen::ArrayXXf gain;
        int nBands, nChannels;

        Example() : Example(Coefficients()) {}
        Example(const Coefficients& c) : algo(c)
        {
            nChannels = c.nChannels;
            nBands = c.nBands;
            snrAposteriori = Eigen::ArrayXXf::Random(nBands, nChannels).abs2() + 1;
            gain = initOutput(snrAposteriori, c);
        }

        void processAlgorithm() { algo.process(snrAposteriori, gain); }
        bool isExampleOutputValid() const 
        { 
            bool test = gain.allFinite();
            test &= (gain >= 0).all();
            test &= (gain.rows() == nBands) && (gain.cols() == nChannels);
            return test;
        }
    };
};

class GainCalculation : public Algorithm<GainCalculationConfiguration>
{
public:
    GainCalculation() = default;
    GainCalculation(const Coefficients& c);
};
