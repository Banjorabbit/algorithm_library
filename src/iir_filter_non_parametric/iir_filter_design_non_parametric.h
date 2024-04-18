#pragma once
#include "algorithm_library/iir_filter_non_parametric.h"
#include "design_iir_non_parametric/design_iir_spline.h"
#include "framework/framework.h"
#include "iir_filter/iir_filter_2nd_order.h"
#include "iir_filter_time_varying/state_variable_filter.h"

// IIR filter where the coefficients are set by giving a set of frequency and gain pairs. The filter is designed using the class DesignIIRSpline
//
// author: Kristian Timm Andersen

// IIR filter is implemented using Transposed Direct Form II filter implemetation
class IIRFilterTDFNonParametric : public AlgorithmImplementation<IIRFilterNonParametricConfiguration, IIRFilterTDFNonParametric>
{
  public:
    IIRFilterTDFNonParametric(Coefficients c = Coefficients())
        : BaseAlgorithm{c}, filterDesignerNonParametric({.nBands = FFTConfiguration::convertFFTSizeToNBands(FFTConfiguration::getValidFFTSize(c.nSos * 16)),
                                                         .nGains = c.nSos,
                                                         .sampleRate = c.sampleRate}) // nBands must be significantly higher than the filter order
    {}

    DesignIIRSpline filterDesignerNonParametric;
    IIRFilterCascaded filter;
    DEFINE_MEMBER_ALGORITHMS(filter, filterDesignerNonParametric)

    void setFilter(I::Real frequencies, I::Real gains)
    {
        Eigen::ArrayXXf sos(6, C.nSos);
        float gain;
        filterDesignerNonParametric.process({frequencies, gains}, {sos, gain});
        filter.setFilter(sos, gain);
    }

    auto getSosFilter() const { return filter.getSosFilter(); }

    float getGain() const { return filter.getGain(); }

  private:
    inline void processAlgorithm(Input input, Output output) { filter.process(input, output); }

    friend BaseAlgorithm;
};

// IIR filter is implemented using State Variable filter implemetation
class IIRFilterSVFNonParametric : public AlgorithmImplementation<IIRFilterNonParametricConfiguration, IIRFilterSVFNonParametric>
{
  public:
    IIRFilterSVFNonParametric(Coefficients c = Coefficients())
        : BaseAlgorithm{c}, filterDesignerNonParametric({.nBands = FFTConfiguration::convertFFTSizeToNBands(FFTConfiguration::getValidFFTSize(c.nSos * 16)),
                                                         .nGains = c.nSos,
                                                         .sampleRate = c.sampleRate}) // nBands must be significantly higher than the filter order
    {
        filter.setFilterTypes({static_cast<long unsigned int>(c.nSos), StateVariableFilter::Parameters::USER_DEFINED});
        gain = Eigen::ArrayXf::Ones(c.nSos);
        cutoff = Eigen::ArrayXf::Constant(c.nSos, std::tan(3.14158f * 0.25f)); // tan(pi * f / fs)
        resonance = Eigen::ArrayXf::Constant(c.nSos, .7071f);                  // 1/sqrt(2) = 0.7071 corresponds to a Butterworth filter
    }

    DesignIIRSpline filterDesignerNonParametric;
    StateVariableFilterCascade filter;
    DEFINE_MEMBER_ALGORITHMS(filter, filterDesignerNonParametric)

    void setFilter(I::Real frequencies, I::Real gains)
    {
        Eigen::ArrayXXf sos(6, C.nSos);
        float g;
        filterDesignerNonParametric.process({frequencies, gains}, {sos, g});
        Eigen::Array3Xf cgr = filter.setUserDefinedSosFilter(sos);
        cutoff = cgr.row(0).transpose();
        gain = cgr.row(1).transpose();
        resonance = cgr.row(2).transpose();
        filter.setGain(g);
    }

    auto getSosFilter() const { return filter.getSosFilter(cutoff, gain, resonance); }

    float getGain() const { return filter.getGain(); }

  private:
    inline void processAlgorithm(Input input, Output output) { filter.process({input, cutoff, gain, resonance}, output); }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = cutoff.getDynamicMemorySize();
        size += gain.getDynamicMemorySize();
        size += resonance.getDynamicMemorySize();
        return size;
    }

    Eigen::ArrayXf cutoff;
    Eigen::ArrayXf gain;
    Eigen::ArrayXf resonance;

    friend BaseAlgorithm;
};
