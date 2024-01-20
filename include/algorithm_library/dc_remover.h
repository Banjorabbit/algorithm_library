#pragma once
#include "interface/interface.h"

// DC remover is a high pass filter that is designed to remove 0 Hz while leaving as much of the remaining bandwidth untouched as possible.
//
// author: Kristian Timm Andersen

// ----------------------------- Cubic interpolation ------------------------------------------------------

struct DCRemoverConfiguration
{

    using Input = I::Real2D;
    using Output = O::Real2D;

    struct Coefficients
    {
        int nChannels = 2;
        float sampleRate = 48000.f;
        DEFINE_TUNABLE_COEFFICIENTS(nChannels, sampleRate)
    };

    struct Parameters
    {
        float cutoffFrequency = 100; // Hz
        DEFINE_TUNABLE_PARAMETERS(cutoffFrequency)
    };

    static auto validInput(Input input, const Coefficients& c) { return (input.rows() > 0) && (input.cols() == c.nChannels); }
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf(input.rows(), c.nChannels); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        int nSamples, nChannels;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;

        Test() : Test(Coefficients()) {}
        Test(const Coefficients& c)
        {
            nSamples = 100;
            nChannels = c.nChannels;
            input.resize(nSamples, nChannels);
            input.setRandom();
            output = initOutput(input, c);
        }

        inline void processAlgorithm() { algo.process(input, output); }
        bool isTestOutputValid() const { return output.allFinite() && (output.rows() == nSamples) && (output.cols() == nChannels); }
    };
};

class DCRemover : public Algorithm<DCRemoverConfiguration>
{
public:
    DCRemover() = default;
    DCRemover(const Coefficients& c);
 };
