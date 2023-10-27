#pragma once
#include "interface/interface.h"

// DC remover is a high pass filter that is designed to remove 0 Hz while leaving as much of the remaining bandwidth untouched as possible.
//
// author: Kristian Timm Andersen

// ----------------------------- Cubic interpolation ------------------------------------------------------

struct DCRemoverConfiguration : public Configuration<I::Real2D, O::Real2D>
{
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
        Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;

        Test(const Coefficients& c = {})
        {
            int nSamples = 100;
            input.resize(nSamples, c.nChannels);
            input.setRandom();
            output = initOutput(input, c);
        }

        inline void processAlgorithm() { algo.process(input, output); }
        bool isTestOutputFinite() const { return output.allFinite(); }
    };
};

class DCRemover : public Algorithm<DCRemoverConfiguration>
{
public:
    DCRemover() = default;
    DCRemover(const Coefficients& c);
 };
