#pragma once
#include "interface/interface.h"

// Filterbank that can be configured in multiple ways.
//
// If nChannels=1, the input to FilterbankAnalysis can be longer than bufferSize and the filterbank will process the input in chunks of bufferSize.
// The output from FilterbankAnalysis will have the size nBands x nFrames, where nFrames = input.size() / bufferSize.
//
// Similarly, if nChannels = 1, the input to FilerbankSynthesis will have the size nBands x nFrames, where nFrames corresponds to the same definition
// of FilterbankAnalysis and the output size will be (bufferSize * nFrames) x 1.
//
// author: Kristian Timm Andersen

struct FilterbankConfiguration
{
    struct Coefficients
    {
        int nChannels = 2;
        int bufferSize = 128;
        int nBands = 257;
        enum FilterbankTypes { HANN, SQRT_HANN, WOLA, USER_DEFINED };
        FilterbankTypes filterbankType = HANN;
        DEFINE_TUNABLE_ENUM(FilterbankTypes, {{HANN, "Hann"}, {SQRT_HANN, "Sqrt Hann"}, {WOLA, "Wola"}, {USER_DEFINED, "User Defined"}})
        DEFINE_TUNABLE_COEFFICIENTS(nChannels, bufferSize, nBands, filterbankType)
    };

    struct Parameters
    {
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    // exception for constructing filterbank with unsupported Configuration
    class ExceptionFilterbank : public std::runtime_error
    {
      public:
        ExceptionFilterbank(const std::string &s, const Coefficients &c)
            : std::runtime_error(std::string("\nException thrown in ") + s + "\nThis configuration is not supported:\n" + convertCoefficientsToJson(c).dump(4) + "\n")
        {}
    };

    static nlohmann::json convertCoefficientsToJson(const Coefficients &c)
    {
        nlohmann::json j(c); // convert c to json
        return j;
    }
};

// --------------------------------------------- Filterbank Analysis ----------------------------------------------------------------

struct FilterbankAnalysisConfiguration : public FilterbankConfiguration
{
    using Input = I::Real2D;
    using Output = O::Complex2D;

    static Eigen::ArrayXXf initInput(const Coefficients &c)
    {
        int timeLength = c.bufferSize;
        if (c.nChannels == 1) { timeLength *= 2; } // arbitrary time samples.
        return Eigen::ArrayXXf::Random(timeLength, c.nChannels);
    }

    static Eigen::ArrayXXcf initOutput(Input input, const Coefficients &c)
    {
        int dim2 = c.nChannels;
        if (c.nChannels == 1) { dim2 = static_cast<int>(input.rows()) / c.bufferSize; }
        return Eigen::ArrayXXcf::Zero(c.nBands, dim2); // frequency bins
    }

    static bool validInput(Input input, const Coefficients &c)
    {
        if ((c.nChannels > 1) && (input.rows() != c.bufferSize)) { return false; }
        return (input.rows() >= c.bufferSize) && (input.cols() == c.nChannels) && input.allFinite();
    }

    static bool validOutput(Output output, const Coefficients &c)
    {
        if ((c.nChannels > 1) && (output.cols() != c.nChannels)) { return false; }
        return ((output.rows() == c.nBands) && (output.cols() > 0) && (output.allFinite()));
    }
};

// Analysis filterbank
class FilterbankAnalysis : public Algorithm<FilterbankAnalysisConfiguration>
{
  public:
    FilterbankAnalysis() = default;
    FilterbankAnalysis(const Coefficients &c);

    float getDelaySamples() const;
};

// --------------------------------------------- Filterbank Synthesis ----------------------------------------------------------------

struct FilterbankSynthesisConfiguration : public FilterbankConfiguration
{
    using Input = I::Complex2D;
    using Output = O::Real2D;

    static Eigen::ArrayXXcf initInput(const Coefficients &c)
    {
        int dim2 = c.nChannels;
        if (c.nChannels == 1) { dim2 = 2; }
        return Eigen::ArrayXXcf::Random(c.nBands, dim2); // frequency bins
    }

    static Eigen::ArrayXXf initOutput(Input input, const Coefficients &c)
    {
        int timeLength = c.bufferSize; // time samples
        if (c.nChannels == 1) { timeLength *= static_cast<int>(input.cols()); }
        return Eigen::ArrayXXf::Zero(timeLength, c.nChannels);
    }

    static bool validInput(Input input, const Coefficients &c)
    {
        if ((c.nChannels > 1) && (input.cols() != c.nChannels)) { return false; }
        return (input.rows() == c.nBands) && (input.cols() > 0) && input.allFinite();
    }

    static bool validOutput(Output output, const Coefficients &c)
    {
        if ((c.nChannels > 1) && (output.rows() != c.bufferSize)) { return false; }
        return (output.rows() >= c.bufferSize) && (output.cols() == c.nChannels) && output.allFinite();
    }
};

class FilterbankSynthesis : public Algorithm<FilterbankSynthesisConfiguration>
{
  public:
    FilterbankSynthesis() = default;
    FilterbankSynthesis(const Coefficients &c);

    float getDelaySamples() const;
};