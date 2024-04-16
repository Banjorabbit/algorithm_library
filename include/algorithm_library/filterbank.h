#pragma once
#include "interface/interface.h"

// Filterbank that can be configured in multiple ways.
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
        ExceptionFilterbank(const Coefficients &c)
            : std::runtime_error(std::string("\nThis configuration is currently not supported:") + "\nbuffer size = " + std::to_string(c.bufferSize) +
                                 "\nnBands = " + std::to_string(c.nBands) + "\nfilterbankType = " + std::string(convertFilterbankTypeToJson(c)) + "\n")
        {}
    };

    static nlohmann::json convertFilterbankTypeToJson(const Coefficients &c)
    {
        nlohmann::json j = c;
        return j["filterbankType"];
    }
};

// --------------------------------------------- Filterbank Analysis ----------------------------------------------------------------

struct FilterbankAnalysisConfiguration : public FilterbankConfiguration
{
    using Input = I::Real2D;
    using Output = O::Complex2D;

    static Eigen::ArrayXXf initInput(const Coefficients &c) { return Eigen::ArrayXXf::Random(c.bufferSize, c.nChannels); } // time samples.

    static Eigen::ArrayXXcf initOutput(Input input, const Coefficients &c) { return Eigen::ArrayXXcf::Zero(c.nBands, c.nChannels); } // frequency bins

    static bool validInput(Input input, const Coefficients &c) { return (input.rows() == c.bufferSize) && (input.cols() == c.nChannels) && input.allFinite(); }

    static bool validOutput(Output output, const Coefficients &c) { return (output.rows() == c.nBands) && (output.cols() == c.nChannels) && output.allFinite(); }
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

    static Eigen::ArrayXXcf initInput(const Coefficients &c) { return Eigen::ArrayXXcf::Random(c.nBands, c.nChannels); } // frequency bins

    static Eigen::ArrayXXf initOutput(Input input, const Coefficients &c) { return Eigen::ArrayXXf::Zero(c.bufferSize, c.nChannels); } // time samples

    static bool validInput(Input input, const Coefficients &c) { return (input.rows() == c.nBands) && (input.cols() == c.nChannels) && input.allFinite(); }

    static bool validOutput(Output output, const Coefficients &c) { return (output.rows() == c.bufferSize) && (output.cols() == c.nChannels) && output.allFinite(); }
};

class FilterbankSynthesis : public Algorithm<FilterbankSynthesisConfiguration>
{
  public:
    FilterbankSynthesis() = default;
    FilterbankSynthesis(const Coefficients &c);

    float getDelaySamples() const;
};