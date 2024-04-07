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
        enum FilterbankTypes { HANN, SQRT_HANN, WOLA};
        FilterbankTypes filterbankType = HANN;
        DEFINE_TUNABLE_ENUM(FilterbankTypes, {{HANN, "Hann"}, {SQRT_HANN, "Sqrt Hann"}, {WOLA, "Wola"}})
        DEFINE_TUNABLE_COEFFICIENTS(nChannels, bufferSize, nBands, filterbankType)
    };

    struct Parameters
    {
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    // exception for constructing filterbank with unsupported Configuration
    class ExceptionFilterbank : public std::runtime_error {
    public:
        ExceptionFilterbank(const Coefficients& c) : 
        std::runtime_error(
        std::string("\nThis configuration is currently not supported:") + 
        "\nbuffer size = " + std::to_string(c.bufferSize) + 
        "\nnBands = " + std::to_string(c.nBands) + 
        "\nfilterbankType = " + std::string(convertFilterbankTypeToJson(c)) +
        "\n") { }
    };

    static nlohmann::json convertFilterbankTypeToJson(const Coefficients& c) { nlohmann::json j = c; return j["filterbankType"];}
};

// --------------------------------------------- Filterbank Analysis ----------------------------------------------------------------

struct FilterbankAnalysisConfiguration : public FilterbankConfiguration
{
    using Input = I::Real2D;
    using Output = O::Complex2D;

    static auto validInput(Input input, const Coefficients& c) { return (input.rows() == c.bufferSize) && (input.cols() == c.nChannels); }
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXcf(c.nBands, c.nChannels); }
    
    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXcf output;
        int nBands, nChannels;

        Example() : Example(Coefficients()) {}
        Example(const Coefficients& c) : algo(c)
        {
            nBands = c.nBands;
            nChannels = c.nChannels;
            input.resize(c.bufferSize, nChannels);
            input.setRandom();
            output = initOutput(input, c);
        }
    
        inline void processAlgorithm() { algo.process(input, output); }
        bool isExampleOutputValid() const { return output.allFinite() && (output.rows() == nBands) && (output.cols() == nChannels); }
    };
};

// Analysis filterbank
class FilterbankAnalysis : public Algorithm<FilterbankAnalysisConfiguration>
{
public:
    FilterbankAnalysis() = default;
    FilterbankAnalysis(const Coefficients& c);

    float getDelaySamples() const;
};

// --------------------------------------------- Filterbank Synthesis ----------------------------------------------------------------

struct FilterbankSynthesisConfiguration : public FilterbankConfiguration
{
    using Input = I::Complex2D;
    using Output = O::Real2D;

    static auto validInput(Input input, const Coefficients& c) { return (input.rows() == c.nBands) && (input.cols() == c.nChannels); }
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf(c.bufferSize, c.nChannels); }

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        Eigen::ArrayXXcf input;
        Eigen::ArrayXXf output;
        int bufferSize, nChannels;

        Example() : Example(Coefficients()) {}
        Example(const Coefficients& c) : algo(c)
        {
            bufferSize = c.bufferSize;
            nChannels = c.nChannels;
            input.resize(c.nBands, nChannels);
            input.setRandom();
            output = initOutput(input, c);
        }

        inline void processAlgorithm() { algo.process(input, output); }
        bool isExampleOutputValid() const { return output.allFinite() && (output.rows() == bufferSize) && (output.cols() == nChannels); }
    };
};

class FilterbankSynthesis : public Algorithm<FilterbankSynthesisConfiguration>
{
public:
    FilterbankSynthesis() = default;
    FilterbankSynthesis(const Coefficients& c);

    float getDelaySamples() const;
};