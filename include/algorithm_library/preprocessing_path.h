#pragma once
#include "interface/interface.h"

struct PreprocessingPathConfiguration
{
    using Input = I::Real2D;
    using Output = O::Real;

    struct Coefficients
    {
        int bufferSize = 128;
        float sampleRate = 16000.f;
        int nChannels = 4;
        BufferMode bufferMode = SINGLE_BUFFER;
        DEFINE_TUNABLE_COEFFICIENTS(bufferSize, sampleRate, nChannels, bufferMode);
    };

    struct Parameters
    {
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    static Eigen::ArrayXXf initInput(const Coefficients& c) { return Eigen::ArrayXXf::Random(c.bufferSize, c.nChannels); } // time samples

    static Eigen::ArrayXf initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXf::Zero(c.bufferSize); } // time samples

    static bool validInput(Input input, const Coefficients& c) 
    { 
        return (input.rows() == c.bufferSize) && (input.cols() == c.nChannels) && input.allFinite();
    }
    
    static bool validOutput(Output output, const Coefficients& c) 
    { 
        return (output.rows() == c.bufferSize) && output.allFinite();
    }

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        Eigen::ArrayXf output;
        int bufferSize, nChannels;

        Example() : Example(Coefficients()) {}
        Example(Coefficients c): algo(c)
        {
            bufferSize = c.bufferSize;
            nChannels = c.nChannels;
            input = Eigen::ArrayXXf::Random(bufferSize, nChannels);
            output = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, output); }
        bool isExampleOutputValid() const 
        { 
            return output.allFinite() && (output.rows() == bufferSize);
        }
    };  
};

class PreprocessingPath : public AlgorithmBuffer<PreprocessingPathConfiguration>
{
public:
    PreprocessingPath() = default;
    PreprocessingPath(const Coefficients& c);
};
