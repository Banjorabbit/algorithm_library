#pragma once
#include "interface/interface.h"

// A parametric equalizer implemented as a cascade of time-varying IIR filters that can be modified at each new time sample.
//
// author: Kristian Timm Andersen

struct IIRFilterTimeVaryingConfiguration
{
    struct Input
    {
        I::Real2D xTime; // each column is a channel. The number of columns must be equal to C.nChannels.
        I::Real2D cutoff; // each column is a cutoff frequency in Hz for the corresponding filter in the cascade section. The number of columns must be equal to C.nSos.
        I::Real2D gain; // each column is a linear gain for the corresponding filter in the cascade section. The number of columns must be equal to C.nSos.
    };

    using Output = O::Real2D; // each column is a channel. The number of columns must be equal to C.nChannels.

    struct Coefficients 
    { 
        int nChannels = 2;
        int nSos = 3;
        float sampleRate = 16000.f;
        DEFINE_TUNABLE_COEFFICIENTS(nChannels, nSos, sampleRate)
    };

    struct Parameters 
    { 
        enum FilterTypes { LowPass, HighPass, BandPass, BandStop, Peaking, LowShelf, HighShelf };
        FilterTypes filterType = LowPass;
        float resonance = 1.f;
        DEFINE_TUNABLE_PARAMETERS (filterType, resonance)
    };

    static auto validInput(Input input, const Coefficients& c) 
    { 
        bool test = (input.xTime.cols() == c.nChannels) && (input.xTime.rows() > 0);
        test &= (input.cutoff.cols() == c.nSos) && (input.cutoff.rows() == input.xTime.rows()) && (input.cutoff >= 0.f).all() && (input.cutoff <= 0.5f * c.sampleRate).all();
        test &= (input.gain.cols() == c.nSos) && (input.gain.rows() == input.xTime.rows()) && (input.gain >= 0.f).all();
        return test;
    }
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf(input.xTime.rows(), c.nChannels); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        Eigen::ArrayXXf xTime;
        Eigen::ArrayXXf cutoff;
        Eigen::ArrayXXf gain;
        Eigen::ArrayXXf output;
        int nChannels, nSos, nSamples;

        Test() : Test(Coefficients()) {}
        Test(const Coefficients& c) : algo(c)
        {
            nChannels = c.nChannels;
            nSos = c.nSos;
            nSamples = 512;
            xTime = Eigen::ArrayXXf::Random(nSamples, nChannels);
            cutoff = 8000.f * Eigen::ArrayXXf::Random(nSamples, nSos).abs2();
            gain = 10.f * Eigen::ArrayXXf::Random(nSamples, nSos).abs2();
            output = initOutput({xTime, cutoff, gain}, c);
        }

        void processAlgorithm() { algo.process({xTime, cutoff, gain}, output); }
        bool isTestOutputValid() const 
        { 
            bool test = output.allFinite();
            test &= (output.rows() == nSamples) && (output.cols() == nChannels);
            return test;
        }
    };
};

class IIRFilterTimeVarying : public Algorithm<IIRFilterTimeVaryingConfiguration>
{
public:
    IIRFilterTimeVarying() = default;
    IIRFilterTimeVarying(const Coefficients& c);

};
