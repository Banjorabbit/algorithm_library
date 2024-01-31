#pragma once
#include "interface/interface.h"

// A parametric equalizer implemented as a cascade of time-varying IIR filters that can be modified at each new time sample.
//
// author: Kristian Timm Andersen

// Configuration of a single time-varying IIR filter that can be modified at each new time sample.
struct IIRFilterTimeVaryingConfiguration
{
    struct Input
    {
        I::Real2D xTime; // each column is a channel. The number of columns must be equal to C.nChannels.
        I::Real cutoff; //cutoff frequency in Hz
        I::Real gain; // linear gain
        I::Real resonance; // resonance/Q-value
    };

     using Output = O::Real2D; // each column is a channel. The number of columns must be equal to C.nChannels.

    struct Coefficients 
    { 
        int nChannels = 2;
        float sampleRate = 16000.f;
        DEFINE_TUNABLE_COEFFICIENTS(nChannels, sampleRate)
    };

    struct Parameters 
    { 
        enum FilterTypes { LowPass, HighPass, BandPass, BandStop, Peaking, LowShelf, HighShelf };
        FilterTypes filterType = LowPass;
        DEFINE_TUNABLE_PARAMETERS (filterType)
    };

    static auto validInput(Input input, const Coefficients& c) 
    { 
        bool test = (input.xTime.cols() == c.nChannels) && (input.xTime.rows() > 0);
        test &= (input.cutoff.rows() == input.xTime.rows()) && (input.cutoff >= 0.f).all() && (input.cutoff <= 0.5f * c.sampleRate).all();
        test &= (input.gain.rows() == input.xTime.rows()) && (input.gain >= 0.f).all();
        test &= (input.resonance.rows() == input.xTime.rows()) && (input.resonance >= 0.f).all();
        return test;
    }

    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf(input.xTime.rows(), c.nChannels); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        Eigen::ArrayXXf xTime;
        Eigen::ArrayXf cutoff;
        Eigen::ArrayXf gain;
        Eigen::ArrayXf resonance;
        Eigen::ArrayXXf output;
        int nChannels, nSamples;

        Test() : Test(Coefficients()) {}
        Test(const Coefficients& c) : algo(c)
        {
            nChannels = c.nChannels;
            nSamples = 512;
            xTime = Eigen::ArrayXXf::Random(nSamples, nChannels);
            cutoff = (8000.f * Eigen::ArrayXf::Random(nSamples).abs2());
            gain = 10.f * Eigen::ArrayXf::Random(nSamples).abs2();
            resonance = 5.f * Eigen::ArrayXf::Random(nSamples).abs2();
            output = initOutput({xTime, cutoff, gain, resonance}, c);
        }

        void processAlgorithm() { algo.process({xTime, cutoff, gain, resonance}, output); }
        bool isTestOutputValid() const 
        { 
            bool test = output.allFinite();
            test &= (output.rows() == nSamples) && (output.cols() == nChannels);
            return test;
        }
    };
};

// A single time-varying IIR filter that can be modified at each new time sample.
class IIRFilterTimeVarying : public Algorithm<IIRFilterTimeVaryingConfiguration>
{
public:
    IIRFilterTimeVarying() = default;
    IIRFilterTimeVarying(const Coefficients& c);

    Eigen::ArrayXf getFilter(float cutoff, float gain, float resonance) const;
    Eigen::ArrayXf getPowerFrequencyResponse(int nBands, float cutoff, float gain, float resonance) const;
};

// Configuration of cascade of time-varying IIR filters that can be modified at each new time sample.
struct IIRFilterCascadeTimeVaryingConfiguration
{
    struct Input
    {
        I::Real2D xTime; // each column is a channel. The number of columns must be equal to C.nChannels.
        I::Real2D cutoff; // each column is a cutoff frequency in Hz for the corresponding filter in the cascade section. The number of columns must be equal to C.nSos.
        I::Real2D gain; // each column is a linear gain for the corresponding filter in the cascade section. The number of columns must be equal to C.nSos.
        I::Real2D resonance; // each column is a resonance/Q-value for the corresponding filter in the cascade section. The number of columns must be equal to C.nSos.
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
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    static auto validInput(Input input, const Coefficients& c) 
    { 
        bool test = (input.xTime.cols() == c.nChannels) && (input.xTime.rows() > 0);
        test &= (input.cutoff.cols() == c.nSos) && (input.cutoff.rows() == input.xTime.rows()) && (input.cutoff >= 0.f).all() && (input.cutoff <= 0.5f * c.sampleRate).all();
        test &= (input.gain.cols() == c.nSos) && (input.gain.rows() == input.xTime.rows()) && (input.gain >= 0.f).all();
        test &= (input.resonance.cols() == c.nSos) && (input.resonance.rows() == input.xTime.rows()) && (input.resonance >= 0.f).all();
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
        Eigen::ArrayXXf resonance;
        Eigen::ArrayXXf output;
        int nChannels, nSos, nSamples;

        Test() : Test(Coefficients()) {}
        Test(const Coefficients& c) : algo(c)
        {
            nChannels = c.nChannels;
            nSos = c.nSos;
            nSamples = 512;
            xTime = Eigen::ArrayXXf::Random(nSamples, nChannels);
            cutoff = (8000.f * Eigen::ArrayXXf::Random(nSamples, nSos).abs2());
            gain = 10.f * Eigen::ArrayXXf::Random(nSamples, nSos).abs2();
            resonance = 5.f * Eigen::ArrayXXf::Random(nSamples, nSos).abs2();
            output = initOutput({xTime, cutoff, gain, resonance}, c);
        }

        void processAlgorithm() { algo.process({xTime, cutoff, gain, resonance}, output); }
        bool isTestOutputValid() const 
        { 
            bool test = output.allFinite();
            test &= (output.rows() == nSamples) && (output.cols() == nChannels);
            return test;
        }
    };
};

// Cascade of time-varying IIR filters that can be modified at each new time sample.
class IIRFilterCascadeTimeVarying : public Algorithm<IIRFilterCascadeTimeVaryingConfiguration>
{
public:
    IIRFilterCascadeTimeVarying() = default;
    IIRFilterCascadeTimeVarying(const Coefficients& c);

    // set/get filter type of all filters in cascade 
    void setFilterTypes(const std::vector<IIRFilterTimeVaryingConfiguration::Parameters::FilterTypes>& vec);
    std::vector<IIRFilterTimeVaryingConfiguration::Parameters::FilterTypes> getFilterTypes() const;

    // set/get filter type of one filter in cascade
    void setFilterType(int index, IIRFilterTimeVaryingConfiguration::Parameters::FilterTypes type);
    IIRFilterTimeVaryingConfiguration::Parameters::FilterTypes getFilterType(int index) const;

    // get overall power frequency response  given filter characteristics (uses internal filter types and sample rate)
    Eigen::ArrayXf getPowerFrequencyResponse(int nBands, I::Real cutoffSos, I::Real gainSos, I::Real resonanceSos) const;

    // get all sos filter coefficients given filter characteristics (uses internal filter types and sample rate)
    Eigen::ArrayXXf getFilter(I::Real cutoffSos, I::Real gainSos, I::Real resonanceSos) const;

    // get overall gain
    float getGain() const;
};