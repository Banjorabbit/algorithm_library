#pragma once
#include "interface/interface.h"
#include "algorithm_library/design_iir_min_phase.h"
#include "min_phase_spectrum/min_phase_spectrum_cepstrum.h"

// Design minimum phase IIR filter from a magnitude spectrum. The IIR filter is factorized in a cascade of 2nd order sections.
//
// TODO: implement algorithm
//
// author: Kristian Timm Andersen
class DesignIIRMinPhaseTF2SOS : public AlgorithmImplementation<DesignIIRMinPhaseConfiguration, DesignIIRMinPhaseTF2SOS>
{
public:
    DesignIIRMinPhaseTF2SOS(const DesignIIRMinPhaseConfiguration::Coefficients& c = Coefficients()) : 
        AlgorithmImplementation<DesignIIRMinPhaseConfiguration, DesignIIRMinPhaseTF2SOS>(c),
        minPhaseCalculator({c.nBands})
    {}

    MinPhaseSpectrumCepstrum minPhaseCalculator;
    DEFINE_MEMBER_ALGORITHMS(minPhaseCalculator)

    void TF2SOS(I::Real tf, O::Real2D sos, O::Float gain)
    {

    }

    void processOn(Input magnitudeSpectrum, Output output)
    {

    }
};