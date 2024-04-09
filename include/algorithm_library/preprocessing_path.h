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
        DEFINE_TUNABLE_COEFFICIENTS(bufferSize, sampleRate, nChannels);
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
};

class PreprocessingPath : public AlgorithmBuffer<PreprocessingPathConfiguration>
{
public:
    PreprocessingPath() = default;
    PreprocessingPath(const Coefficients& c);
};
