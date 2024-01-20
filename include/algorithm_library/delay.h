#pragma once 
#include "interface/interface.h"

struct DelayConfiguration
{
    using Input = I::Real2D;
    using Output = O::Real2D;

    struct Coefficients
    {
        int nChannels = 2;
        int delayLength = 100;
        DEFINE_TUNABLE_COEFFICIENTS(nChannels, delayLength)
    };

    struct Parameters { DEFINE_NO_TUNABLE_PARAMETERS };

    static auto validInput(Input input, const Coefficients& c) 
    { 
        return (input.rows() > 0) && (input.cols() == c.nChannels);
    }
    
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf(input.rows(), c.nChannels); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;
        int delayLength, nChannels;

        Test() : Test(Coefficients()) {}
        Test(const Coefficients& c) : algo(c)
        {
            delayLength = c.delayLength;
            nChannels = c.nChannels;
            input = Eigen::ArrayXXf(delayLength, nChannels);
            output = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, output); }
        bool isTestOutputValid() const { return output.allFinite() && (output.rows() == delayLength) && (output.cols() == nChannels); }
    };
};

class Delay : public Algorithm<DelayConfiguration>
{
public:
    Delay() = default;
    Delay(const Coefficients& c);

    void push(Input input);
    void pop(Output output);

    float get(int index, int channel = 0);
    float get(float index, int channel = 0);
};