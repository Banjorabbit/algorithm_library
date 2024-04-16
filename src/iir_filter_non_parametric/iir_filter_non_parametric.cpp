#include "iir_filter_non_parametric/iir_filter_design_non_parametric.h"

using IIRFilterTDFNonParametricSingleBufferImpl = Implementation<IIRFilterTDFNonParametric, IIRFilterNonParametricConfiguration>;
using IIRFilterSVFNonParametricSingleBufferImpl = Implementation<IIRFilterSVFNonParametric, IIRFilterNonParametricConfiguration>;

template <>
Algorithm<IIRFilterNonParametricConfiguration>::Algorithm(const Coefficients &c)
{
    if (c.filterImplementation == c.TDF) { pimpl = std::make_unique<IIRFilterTDFNonParametricSingleBufferImpl>(c); }
    else { pimpl = std::make_unique<IIRFilterSVFNonParametricSingleBufferImpl>(c); }
}

IIRFilterNonParametric::IIRFilterNonParametric(const Coefficients &c) : Algorithm<IIRFilterNonParametricConfiguration>(c) {}

void IIRFilterNonParametric::setFilter(I::Real frequencies, I::Real gains)
{
    if (getCoefficients().filterImplementation == Coefficients::TDF)
    {
        static_cast<IIRFilterTDFNonParametricSingleBufferImpl *>(pimpl.get())->algo.setFilter(frequencies, gains);
    }
    else { static_cast<IIRFilterSVFNonParametricSingleBufferImpl *>(pimpl.get())->algo.setFilter(frequencies, gains); }
}

Eigen::ArrayXXf IIRFilterNonParametric::getFilter() const
{
    if (getCoefficients().filterImplementation == Coefficients::TDF) { return static_cast<IIRFilterTDFNonParametricSingleBufferImpl *>(pimpl.get())->algo.getFilter(); }
    else { return static_cast<IIRFilterSVFNonParametricSingleBufferImpl *>(pimpl.get())->algo.getFilter(); }
}

float IIRFilterNonParametric::getGain() const
{
    if (getCoefficients().filterImplementation == Coefficients::TDF) { return static_cast<IIRFilterTDFNonParametricSingleBufferImpl *>(pimpl.get())->algo.getGain(); }
    else { return static_cast<IIRFilterSVFNonParametricSingleBufferImpl *>(pimpl.get())->algo.getGain(); }
}

// get power frequency response evaluated uniformly from 0 to pi in nBands points
Eigen::ArrayXf IIRFilterNonParametric::getPowerFrequencyResponse(int nBands) const
{
    if (getCoefficients().filterImplementation == Coefficients::TDF)
    {
        return static_cast<IIRFilterTDFNonParametricSingleBufferImpl *>(pimpl.get())->algo.getPowerFrequencyResponse(nBands);
    }
    else { return static_cast<IIRFilterSVFNonParametricSingleBufferImpl *>(pimpl.get())->algo.getPowerFrequencyResponse(nBands); }
}