#pragma once
#include "interface/interface.h"

// Design minimum phase IIR filter from an array of frequency and gain pairs. The IIR filter is factorized in a cascade of 2nd order sections.
//
// author: Kristian Timm Andersen


struct DesignIIRNonParametricConfiguration
{
    struct Input
    {
        I::Real frequencies;
        I::Real gaindB;
    };
     
    // sos is structured as:
    // [b0 bb0 ... ]
    // [b1 bb1 ... ]
    // [b2 bb2 ... ]
    // [1  1   ... ]
    // [a1 aa1 ... ]
    // [a2 aa2 ... ]
    struct Output
    {
        O::Real6X sos;
        O::Float gain;
    };

    struct Coefficients
    {
        int nBands = 257; // nBands should be significantly higher than the filter order: nOrder
        int nGains = 5; // number of frequency/gain pairs. High orders are not numerically stable when calculating roots (about >64)
        float sampleRate = 16000;
        DEFINE_TUNABLE_COEFFICIENTS(nBands, nGains, sampleRate)
    };

    struct Parameters
    {
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    static auto validInput(Input input, const Coefficients& c) { return (input.frequencies.rows() > 0) && (input.frequencies > 0).all() && (input.frequencies.rows() == input.gaindB.rows()); }
    static auto initOutput(Input input, const Coefficients& c) { return std::make_tuple(Eigen::Array<float, 6, Eigen::Dynamic>::Zero(6, c.nGains), float()); }

    template<typename Talgo>
    struct Example
    {
        Talgo algo;
        int nBands, nGains;
        Eigen::ArrayXf frequencies, gaindB;
        Eigen::Array<float, 6, Eigen::Dynamic> sos;
        float gain;

        Example() : Example(Coefficients()) {}
        Example(const Coefficients& c) : algo(c)
        {
            nBands = c.nBands;
            nGains = c.nGains;
            frequencies.resize(nGains);
            gaindB.resize(nGains);
            frequencies << 80, 500, 1000, 2400, 4000;
            gaindB << 5, 10, -10, 0, 7;
            std::tie(sos, gain) = initOutput({frequencies, gaindB}, c);
        }

        void processAlgorithm() { algo.process({frequencies, gaindB}, {sos, gain}); }
        bool isExampleOutputValid() const { return sos.allFinite() && std::isfinite(gain) && (sos.cols() == nGains) && (sos.rows() == 6); }
    };
};

class DesignIIRNonParametric : public Algorithm<DesignIIRNonParametricConfiguration>
{
public:
    DesignIIRNonParametric() = default;
    DesignIIRNonParametric(const Coefficients& c);
};

