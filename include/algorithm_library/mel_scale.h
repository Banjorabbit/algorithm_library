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

    static Eigen::ArrayXXf initInput(const Coefficients& c) { return Eigen::ArrayXXf::Random(c.nBands, 2).abs2(); } // power spectrum. number of channels is arbitrary

    static Eigen::ArrayXXf initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf::Zero(c.nMels, input.cols()); } // mel-scale power spectrum

    static bool validInput(Input input, const Coefficients& c) { return (input.cols() > 0) && (input.rows() == c.nBands) && (input >= 0).all();	}
    
    static bool validOutput(Output output, const Coefficients& c) { return (output.cols() > 0) && (output.rows() == c.nMels) && (output >= 0).all(); }

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;
        int nMels, nChannels;

        Example() : Example(Coefficients()) {}
        Example(const Coefficients& c) : algo(c)
        {
            nMels = c.nMels;
            nChannels = 2;
            input = Eigen::ArrayXXf::Random(512, nChannels).abs2();
            output = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, output); }
        bool isExampleOutputValid() const 
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

    Eigen::ArrayXf getCornerFrequencies() const;
};
