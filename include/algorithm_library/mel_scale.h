#pragma once
#include "interface/interface.h"

// Mel scale
//
// author: Kristian Timm Andersen

struct MelScaleConfiguration
{
    using Input = I::Real2D;
    using Output = O::Real2D;

    struct Coefficients 
    { 
        int nMels = 40; // output size
        int nBands = 513; // input size
        float sampleRate = 16000; // Hz
        DEFINE_TUNABLE_COEFFICIENTS(nMels, nBands, sampleRate)
    };

    struct Parameters { DEFINE_NO_TUNABLE_PARAMETERS };

    static auto validInput(Input input, const Coefficients& c) { return (input >= 0).all() && (input.cols() > 0) && (input.rows() == c.nBands);	}
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf(c.nMels, input.cols()); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;
        int nMels, nChannels;

        Test() : Test(Coefficients()) {}
        Test(const Coefficients& c) : algo(c)
        {
            nMels = c.nMels;
            nChannels = 2;
            input = Eigen::ArrayXXf::Random(512, nChannels).abs2();
            output = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, output); }
        bool isTestOutputValid() const 
        { 
            bool test = output.allFinite();
            test &= (output >= 0).all();
            test &= (output.rows() == nMels) && (output.cols() == nChannels);
            return test;
        }
    };
};

class MelScale : public Algorithm<MelScaleConfiguration>
{
public:
    MelScale() = default;
    MelScale(const Coefficients& c);
};
