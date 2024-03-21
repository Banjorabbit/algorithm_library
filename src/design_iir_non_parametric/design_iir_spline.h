#pragma once
#include "framework/framework.h"
#include "algorithm_library/design_iir_non_parametric.h"
#include "design_iir_min_phase/design_iir_min_phase_tf2sos.h"
#include "spline/spline_cubic.h"
#include "utilities/fastonebigheader.h"

// Design minimum phase IIR filter from an array of frequency and gain pairs. The IIR filter is factorized in a cascade of 2nd order sections.
//
// author: Kristian Timm Andersen
class DesignIIRSpline : public AlgorithmImplementation<DesignIIRNonParametricConfiguration, DesignIIRSpline>
{
public:
    DesignIIRSpline(const DesignIIRNonParametricConfiguration::Coefficients& c = Coefficients()) :
        AlgorithmImplementation<DesignIIRNonParametricConfiguration, DesignIIRSpline>(c),
        splineCalculation({c.nGains + 4}),
        filterDesigner(convertToDesignIIRMinPhaseTF2SOS(c))
    { 
        // add 2 frequency/gain points to the left and right of given points to ensure correct gradient 
        frequencies.resize(c.nGains + 4);
        gaindB.resize(c.nGains + 4);
        freqsFFT = Eigen::ArrayXf::LinSpaced(C.nBands, 0.f, C.sampleRate / 2); // desired frequency points for FFT
        gainFFT.resize(C.nBands);
    }

    SplineCubic splineCalculation;
    DesignIIRMinPhaseTF2SOS filterDesigner;
    DEFINE_MEMBER_ALGORITHMS(splineCalculation, filterDesigner)

private:

    void processOn(Input input, Output output)
    {
        frequencies.head(2) = -input.frequencies.head(2).reverse();
        frequencies.segment(2, input.frequencies.rows()) = input.frequencies;
        frequencies.tail(2) = C.sampleRate - input.frequencies.tail(2).reverse();

        gaindB.head(2) = input.gaindB.head(2).reverse();
        gaindB.segment(2, input.gaindB.rows()) = input.gaindB;
        gaindB.tail(2) = input.gaindB.tail(2).reverse();
        
        // calculate interpolation
        splineCalculation.process({ frequencies, gaindB, freqsFFT }, gainFFT);
        // convert to linear scale
        gainFFT *= 0.166096404744368f; // 10^(gainFFT/20) = 2^(log2(10^(gainFFT/20))) = 2^(gainFFT/20*log2(10)) = 2^(gainFFT*0.166096404744368)
        gainFFT = gainFFT.unaryExpr(std::ref(fasterpow2));

        // calculate minimum phase spectrum
        filterDesigner.process(gainFFT, {output.sos, output.gain});
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = frequencies.getDynamicMemorySize();
        size += gaindB.getDynamicMemorySize();
        size += freqsFFT.getDynamicMemorySize();
        size += gainFFT.getDynamicMemorySize();
        return size;
    }

    DesignIIRMinPhaseTF2SOS::Coefficients convertToDesignIIRMinPhaseTF2SOS(const Coefficients& c)
    {
        DesignIIRMinPhaseTF2SOS::Coefficients cMinPhase;
        cMinPhase.nOrder = 2 * c.nGains;
        cMinPhase.nBands = c.nBands;
        cMinPhase.sampleRate = c.sampleRate;
        cMinPhase.weightType = cMinPhase.MELSCALE;
        return cMinPhase;
    }

    Eigen::ArrayXf frequencies;
    Eigen::ArrayXf gaindB;
    Eigen::ArrayXf freqsFFT; // desired frequency points for FFT
    Eigen::ArrayXf gainFFT;

    friend AlgorithmImplementation<DesignIIRNonParametricConfiguration, DesignIIRSpline>;
};