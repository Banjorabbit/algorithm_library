#pragma once
#include "interface/interface.h"

// Design minimum phase IIR filter from a magnitude spectrum. The IIR filter is factorized in a cascade of 2nd order sections.
//
// author: Kristian Timm Andersen


struct DesignIIRMinPhaseConfiguration
{
    using Input = I::Real;
 
    // sos is structured as:
    // [b0 bb0 ... ]
    // [b1 bb1 ... ]
    // [b2 bb2 ... ]
    // [a0 aa0 ... ]
    // [a1 aa1 ... ]
    // [a2 aa2 ... ]
    struct Output
    {
        O::RealX6 sos;
        O::Float gain;
    };

    struct Coefficients
    {
        int nBands = 257; // nBands should be significantly higher than the filter order: nOrder
        float sampleRate = 16000;
        int nOrder = 8; // filter order. High orders are not numerically stable when calculating roots (about >32)
        enum WeightTypes {LINEAR, MELSCALE};
        WeightTypes weightType = MELSCALE;
        DEFINE_TUNABLE_COEFFICIENTS(nBands, sampleRate, nOrder, weightType)
    };

    struct Parameters
    {
        DEFINE_NO_TUNABLE_PARAMETERS
    };

    static int getNSos(int nOrder) { return static_cast<int>(std::ceil(static_cast<float>(nOrder) / 2)); } // get number of second-order sections from filter order

    static auto validInput(Input input, const Coefficients& c) { return input.rows() == c.nBands; }
    static auto initOutput(Input input, const Coefficients& c) { return std::make_tuple(Eigen::Array<float, Eigen::Dynamic, 6>::Zero(getNSos(c.nOrder), 6), float()); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        int nBands, nOrder;
        Eigen::ArrayXf magnitudeSpectrum;
        Eigen::Array<float, Eigen::Dynamic, 6> sos;
        float gain;

        Test() : Test(Coefficients()) {}
        Test(const Coefficients& c) : algo(c)
        {
            nBands = c.nBands;
            nOrder = c.nOrder;
            magnitudeSpectrum = Eigen::ArrayXf::Random(nBands).abs2();
            std::tie(sos,gain) = initOutput(magnitudeSpectrum, c);
        }

        void processAlgorithm() { algo.process(magnitudeSpectrum, {sos, gain}); }
        bool isTestOutputValid() const { return sos.allFinite() && std::isfinite(gain) && (sos.rows() == getNSos(nOrder)) && (sos.cols() == 6); }
    };
};

class DesignIIRMinPhase : public Algorithm<DesignIIRMinPhaseConfiguration>
{
public:
    DesignIIRMinPhase() = default;
    DesignIIRMinPhase(const Coefficients& c);

    int getNSos() const { return Configuration::getNSos(getCoefficients().nOrder); }
};

