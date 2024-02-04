#pragma once
#include "interface/interface.h"

// IIR filter where the coefficients are set by giving a set of frequency and gain pairs.
//
// author: Kristian Timm Andersen

struct IIRFilterNonParametricConfiguration
{
    using Input = I::Real2D;
    using Output = O::Real2D;

    struct Coefficients 
    { 
        int nChannels = 2;
        int nSos = 3;
        float sampleRate = 16000.f;
        DEFINE_TUNABLE_COEFFICIENTS(nChannels, nSos)
    };

    struct Parameters { DEFINE_NO_TUNABLE_PARAMETERS };

    static auto validInput(Input input, const Coefficients& c) { return (input.cols() == c.nChannels) && (input.rows() > 0); }
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf(input.rows(), c.nChannels); }

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;
        int nChannels;

        Example() : Example(Coefficients()) {}
        Example(const Coefficients& c) : algo(c)
        {
            nChannels = 2;
            input = Eigen::ArrayXXf::Random(512, nChannels);
            output = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, output); }
        bool isExampleOutputValid() const 
        { 
            bool test = output.allFinite();
            test &= (output.rows() > 0) && (output.cols() == nChannels);
            return test;
        }
    };
};

class IIRFilterNonParametric : public Algorithm<IIRFilterNonParametricConfiguration>
{
public:
    IIRFilterNonParametric() = default;
    IIRFilterNonParametric(const Coefficients& c);

    // set of frequency and gain pairs
    void setFilter(I::Real frequencies, I::Real gains);

    // sos filter is structured as:
    // [b0 bb0 ... ]
    // [b1 bb1 ... ]
    // [b2 bb2 ... ]
    // [1  1   ... ]
    // [a1 aa1 ... ]
    // [a2 aa2 ... ]
    Eigen::ArrayXXf getFilter() const;
    float getGain() const;

    // get power frequency response evaluated uniformly from 0 to pi in nBands points
    Eigen::ArrayXf getPowerFrequencyResponse(int nBands);
};