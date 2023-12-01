#pragma once
#include "interface/interface.h"

struct PreprocessingPathConfiguration
{
    using Input = I::Real2D;
    using Output = O::Real2D;

    struct Coefficients
    {
        int bufferSize = 128;
        float sampleRate = 48000.f;
        int nChannels = 2;
        BufferMode bufferMode = SINGLE_BUFFER;
        DEFINE_TUNABLE_COEFFICIENTS(bufferSize, sampleRate, nChannels, bufferMode);
    };

    struct Parameters
    {
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    static auto validInput(Input input, const Coefficients& c) 
    { 
        if (c.bufferMode == SINGLE_BUFFER)
        {
            return (input.rows() == c.bufferSize) && (input.cols() == c.nChannels);
        }
        return (input.cols() == c.nChannels);
    }
    
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXf(input.rows()); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        Eigen::ArrayXf output;

        Test() : Test(Coefficients()) {}
        Test(Coefficients c): algo(c)
        {
            input = Eigen::ArrayXXf::Random(c.bufferSize, c.nChannels);
            output = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, output); }
        bool isTestOutputFinite() const { return output.allFinite(); }
    };  
};

class PreprocessingPath : public AlgorithmBuffer<PreprocessingPathConfiguration>
{
public:
    PreprocessingPath() = default;
    PreprocessingPath(const Coefficients& c);
};
