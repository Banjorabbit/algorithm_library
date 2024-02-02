#pragma once
#include "framework/framework.h"
#include "algorithm_library/iir_filter_non_parametric.h"
#include "design_iir_non_parametric/design_iir_spline.h"
#include "iir_filter/iir_filter_2nd_order.h"

// IIR filter where the coefficients are set by giving a set of frequency and gain pairs. The filter is designed using the class DesignIIRSpline
//
// author: Kristian Timm Andersen

// IIR filter is implemented using Transposed Direct Form II filter implemetation
class IIRFilterTDFNonParametric : public AlgorithmImplementation<IIRFilterNonParametricConfiguration, IIRFilterTDFNonParametric>
{
public:
    IIRFilterTDFNonParametric(Coefficients c = Coefficients()) :
        AlgorithmImplementation<IIRFilterNonParametricConfiguration, IIRFilterTDFNonParametric>{ c },
        filterDesigner(convertToDesignIIRSplineCoefficients(c)),
        filter({ c.nChannels, c.nSos})
    { }

    DesignIIRSpline filterDesigner;
    IIRFilterCascaded filter;
    DEFINE_MEMBER_ALGORITHMS(filter, filterDesigner)

    inline void processOn(Input input, Output output)
    {
        filter.process(input, output);
    }

    void setFilter(I::Real frequencies, I::Real gains)
    {
        Eigen::ArrayXXf sos(6, C.nSos);
        float gain;
        filterDesigner.process({frequencies, gains}, {sos, gain});
        filter.setFilter(sos, gain);
    }

    // get power frequency response evaluated uniformly from 0 to pi in nBands points
    auto getPowerFrequencyResponse(int nBands) const
    {
        return filter.getPowerFrequencyResponse(nBands);
    }

    auto getFilter() const
    {
        return filter.getFilter();
    }

private:

    DesignIIRSpline::Coefficients convertToDesignIIRSplineCoefficients(const Coefficients & c)
    {
        DesignIIRSpline::Coefficients coefficients;
        coefficients.nOrder = c.nSos * 2;
        coefficients.nBands = FFTReal::getValidFFTSize(c.nSos * 16)/2+1; // must be significantly higher than the filter order
        coefficients.sampleRate = c.sampleRate;
        return coefficients;
    }
};
