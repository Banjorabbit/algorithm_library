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
        I::Real2D xTime;   // each column is a channel. The number of columns must be equal to C.nChannels.
        I::Real cutoff;    // prewarped cutoff frequency in Hz: cutoff = tan(pi*f/fs)
        I::Real gain;      // linear gain
        I::Real resonance; // resonance/Q-value
    };

    using Output = O::Real2D; // each column is a channel. The number of columns must be equal to C.nChannels.

    struct Coefficients
    {
        int nChannels = 2;
        DEFINE_TUNABLE_COEFFICIENTS(nChannels)
    };

    struct Parameters
    {
        enum FilterTypes { LOWPASS, HIGHPASS, BANDPASS, BANDSTOP, PEAKING, LOWSHELF, HIGHSHELF, USER_DEFINED };
        FilterTypes filterType = LOWPASS;
        DEFINE_TUNABLE_ENUM(FilterTypes, {{LOWPASS, "LowPass"},
                                          {HIGHPASS, "HighPass"},
                                          {BANDPASS, "BandPass"},
                                          {BANDSTOP, "BandStop"},
                                          {PEAKING, "Peaking"},
                                          {LOWSHELF, "LowShelf"},
                                          {HIGHSHELF, "HighShelf"},
                                          {USER_DEFINED, "UserDefined"}})
        DEFINE_TUNABLE_PARAMETERS(filterType)
    };

    static std::tuple<Eigen::ArrayXXf, Eigen::ArrayXf, Eigen::ArrayXf, Eigen::ArrayXf> initInput(const Coefficients &c)
    {
        int nSamples = 100;                                                                                    // Number of samples can be arbitrary
        Eigen::ArrayXXf xTime = Eigen::ArrayXXf::Random(nSamples, c.nChannels);                                // time samples
        Eigen::ArrayXf cutoff = (3.14159f * 8000.f / 16000.f * Eigen::ArrayXf::Random(nSamples).abs2()).tan(); // cutoff frequency in Hz prewarped: tan(pi * f/fs)
        Eigen::ArrayXf gain = 10.f * Eigen::ArrayXf::Random(nSamples).abs2();                                  // linear gain
        Eigen::ArrayXf resonance = 5.f * Eigen::ArrayXf::Random(nSamples).abs2();                              // resonance/Q-value
        return std::make_tuple(xTime, cutoff, gain, resonance);
    }

    static Eigen::ArrayXXf initOutput(Input input, const Coefficients &c) { return Eigen::ArrayXXf::Zero(input.xTime.rows(), c.nChannels); } // time samples

    static bool validInput(Input input, const Coefficients &c)
    {
        bool test = (input.xTime.cols() == c.nChannels) && (input.xTime.rows() > 0) && input.xTime.allFinite();
        test &= (input.cutoff.rows() == input.xTime.rows()) && (input.cutoff >= 0.f).all();
        test &= (input.gain.rows() == input.xTime.rows()) && (input.gain >= 0.f).all();
        test &= (input.resonance.rows() == input.xTime.rows()) && (input.resonance > 0.f).all();
        return test;
    }

    static bool validOutput(Output output, const Coefficients &c) { return (output.rows() > 0) && (output.cols() == c.nChannels); }
};

// A single time-varying IIR filter that can be modified at each new time sample.
class IIRFilterTimeVarying : public Algorithm<IIRFilterTimeVaryingConfiguration>
{
  public:
    IIRFilterTimeVarying() = default;
    IIRFilterTimeVarying(const Coefficients &c);

    // return filter as second-order section
    Eigen::ArrayXf getSosFilter(float cutoff, float gain, float resonance) const;

    // Given a second order section of the type:
    // sos = [b0 b1, b2, 1.f, a1, a2]
    // set P.filterType to USER_DEFINED and convert sos to internal representation. Return cutoff, gain and resonance in an array.
    Eigen::Array3f setUserDefinedSosFilter(I::Real sos);
};

// Configuration of cascade of time-varying IIR filters that can be modified at each new time sample.
struct IIRFilterCascadeTimeVaryingConfiguration
{
    struct Input
    {
        I::Real2D xTime;     // each column is a channel. The number of columns must be equal to C.nChannels.
        I::Real2D cutoff;    // each column is a prewarped cutoff frequency in Hz for the corresponding filter in the cascade section: cutoff = tan(pi*f/fs). The number of
                             // columns must be equal to C.nSos.
        I::Real2D gain;      // each column is a linear gain for the corresponding filter in the cascade section. The number of columns must be equal to C.nSos.
        I::Real2D resonance; // each column is a resonance/Q-value for the corresponding filter in the cascade section. The number of columns must be equal to C.nSos.
    };

    using Output = O::Real2D; // each column is a channel. The number of columns must be equal to C.nChannels.

    struct Coefficients
    {
        int nChannels = 2;
        int nSos = 3;
        DEFINE_TUNABLE_COEFFICIENTS(nChannels, nSos)
    };

    struct Parameters
    {
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    static std::tuple<Eigen::ArrayXXf, Eigen::ArrayXXf, Eigen::ArrayXXf, Eigen::ArrayXXf> initInput(const Coefficients &c)
    {
        int nSamples = 100;                                                                                              // Number of samples can be arbitrary
        Eigen::ArrayXXf xTime = Eigen::ArrayXXf::Random(nSamples, c.nChannels);                                          // time samples
        Eigen::ArrayXXf cutoff = (3.14159f * 8000.f / 16000.f * Eigen::ArrayXXf::Random(nSamples, c.nSos).abs2()).tan(); // cutoff frequency in Hz prewarped: tan(pi * f/fs)
        Eigen::ArrayXXf gain = 10.f * Eigen::ArrayXXf::Random(nSamples, c.nSos).abs2();                                  // linear gain
        Eigen::ArrayXXf resonance = 5.f * Eigen::ArrayXXf::Random(nSamples, c.nSos).abs2();                              // resonance/Q-value
        return std::make_tuple(xTime, cutoff, gain, resonance);
    }

    static Eigen::ArrayXXf initOutput(Input input, const Coefficients &c) { return Eigen::ArrayXXf::Zero(input.xTime.rows(), c.nChannels); } // time samples

    static bool validInput(Input input, const Coefficients &c)
    {
        bool test = (input.xTime.cols() == c.nChannels) && (input.xTime.rows() > 0) && input.xTime.allFinite();
        test &= (input.cutoff.rows() == input.xTime.rows()) && (input.cutoff.cols() == c.nSos) && (input.cutoff >= 0.f).all();
        test &= (input.gain.rows() == input.xTime.rows()) && (input.gain.cols() == c.nSos) && (input.gain >= 0.f).all();
        test &= (input.resonance.rows() == input.xTime.rows()) && (input.resonance.cols() == c.nSos) && (input.resonance > 0.f).all();
        return test;
    }

    static bool validOutput(Output output, const Coefficients &c) { return (output.rows() > 0) && (output.cols() == c.nChannels); }
};

// Cascade of time-varying IIR filters that can be modified at each new time sample.
class IIRFilterCascadeTimeVarying : public Algorithm<IIRFilterCascadeTimeVaryingConfiguration>
{
  public:
    IIRFilterCascadeTimeVarying() = default;
    IIRFilterCascadeTimeVarying(const Coefficients &c);

    // set/get filter type of all filters in cascade
    void setFilterTypes(const std::vector<IIRFilterTimeVaryingConfiguration::Parameters::FilterTypes> &vec);
    std::vector<IIRFilterTimeVaryingConfiguration::Parameters::FilterTypes> getFilterTypes() const;

    // set/get filter type of one filter in cascade
    void setFilterType(int index, IIRFilterTimeVaryingConfiguration::Parameters::FilterTypes type);
    IIRFilterTimeVaryingConfiguration::Parameters::FilterTypes getFilterType(int index) const;

    // get all sos filter coefficients given filter characteristics (uses internal filter types)
    Eigen::ArrayXXf getSosFilter(I::Real cutoffSos, I::Real gainSos, I::Real resonanceSos) const;

    // get overall gain
    float getGain() const;

    // Given a second order section of the type:
    // [b0 bb0 ... ]
    // [b1 bb1 ... ]
    // [b2 bb2 ... ]
    // [1  1   ... ]
    // [a1 aa1 ... ]
    // [a2 aa2 ... ]
    // set P.filterType to USER_DEFINED, return cutoff, resonance and gain, and set cLP, cBP and cHP for each second order section
    Eigen::Array3Xf setUserDefinedSosFilter(I::Real2D sos);
};