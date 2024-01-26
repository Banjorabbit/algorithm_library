#pragma once
#include "framework/framework.h"
#include "algorithm_library/design_iir_non_parametric.h"
#include "design_iir_min_phase/design_iir_min_phase_tf2sos.h"
#include "spline/spline_cubic.h"
#include "utilities/functions.h"

// Design minimum phase IIR filter from an array of frequency and gain pairs. The IIR filter is factorized in a cascade of 2nd order sections.
//
// author: Kristian Timm Andersen
class DesignIIRSpline : public AlgorithmImplementation<DesignIIRNonParametricConfiguration, DesignIIRSpline>
{
public:
    DesignIIRSpline(const DesignIIRNonParametricConfiguration::Coefficients& c = Coefficients()) :
        AlgorithmImplementation<DesignIIRNonParametricConfiguration, DesignIIRSpline>(c),
        splineCalculation(),
        filterDesigner(convertToDesignIIRMinPhaseTF2SOS(c))
    { }

	SplineCubic splineCalculation;
	DesignIIRMinPhaseTF2SOS filterDesigner;
    DEFINE_MEMBER_ALGORITHMS(splineCalculation, filterDesigner)

	void processOn(Input input, Output output)
	{
        // add 2 frequency/gain points to the left and right of given points to ensure correct gradient 
        Eigen::ArrayXf frequencies(input.frequencies.rows() + 4);
        frequencies.head(2) = -input.frequencies.head(2).reverse();
        frequencies.segment(2, input.frequencies.rows()) = input.frequencies;
        frequencies.tail(2) = C.sampleRate - input.frequencies.tail(2).reverse();
        Eigen::ArrayXf gaindB(input.gaindB.rows() + 4);
        gaindB.head(2) = input.gaindB.head(2).reverse();
        gaindB.segment(2, input.gaindB.rows()) = input.gaindB;
        gaindB.tail(2) = input.gaindB.tail(2).reverse();
        
        // calculate interpolation
        Eigen::ArrayXf freqsFFT = Eigen::ArrayXf::LinSpaced(C.nBands, 0.f, C.sampleRate / 2); // desired frequency points for FFT
        Eigen::ArrayXf gainFFT(C.nBands);
        splineCalculation.process({ frequencies, gaindB, freqsFFT }, gainFFT);
        // convert to linear scale
        gainFFT *= 0.115129254649702f; // 10^(gainFFT/20) = e^(log(10^(gainFFT/20))) = e^(gainFFT/20*log(10)) = e^(gainFFT*0.115129254649702)
        gainFFT = gainFFT.unaryExpr(std::ref(fasterExp));

        // calculate minimum phase spectrum
        filterDesigner.process(gainFFT, {output.sos, output.gain});
	}

private:

    DesignIIRMinPhaseTF2SOS::Coefficients convertToDesignIIRMinPhaseTF2SOS(const Coefficients& c)
    {
        DesignIIRMinPhaseTF2SOS::Coefficients cMinPhase;
        cMinPhase.nOrder = c.nOrder;
        cMinPhase.nBands = c.nBands;
        cMinPhase.sampleRate = c.sampleRate;
        cMinPhase.weightType = cMinPhase.MELSCALE;
        return cMinPhase;
    }

};