#include "iir_filter/iir_filter_2nd_order.h"

template<> 
Algorithm<IIRFilterConfiguration>::Algorithm(const Coefficients& c) 
{ 
    if (c.nSos == 1) 
    {
        pimpl = std::make_unique<Implementation<IIRFilter2ndOrder, IIRFilterConfiguration>>(c); 
    }
    else
    {
        pimpl = std::make_unique<Implementation<IIRFilterCascaded, IIRFilterConfiguration>>(c);
    }
} 

IIRFilter::IIRFilter(const Coefficients& c) : Algorithm<IIRFilterConfiguration>(c) 
{}

// sos is structured as:
// [b0 bb0 ... ]
// [b1 bb1 ... ]
// [b2 bb2 ... ]
// [a0 aa0 ... ]
// [a1 aa1 ... ]
// [a2 aa2 ... ]
void IIRFilter::setFilter(I::Real2D sos, float gain)
{
    if (getCoefficients().nSos == 1)
    {
        Eigen::ArrayXf sos1 = sos.col(0); // filter only has 1 sos
        sos1.head(3) *= gain; // embed gain into sos
        static_cast<Implementation<IIRFilter2ndOrder, IIRFilterConfiguration>*>(pimpl.get())->algo.setFilter(sos);
    }
    else
    {
        static_cast<Implementation<IIRFilterCascaded, IIRFilterConfiguration>*>(pimpl.get())->algo.setFilter(sos, gain);
    }
}

// get power frequency response evaluated uniformly from 0 to pi in nBands points
Eigen::ArrayXf IIRFilter::getPowerFrequencyResponse(int nBands)
{
    if (getCoefficients().nSos == 1)
    {
        return static_cast<Implementation<IIRFilter2ndOrder, IIRFilterConfiguration>*>(pimpl.get())->algo.getPowerFrequencyResponse(nBands);
    }
    else
    {
        return static_cast<Implementation<IIRFilterCascaded, IIRFilterConfiguration>*>(pimpl.get())->algo.getPowerFrequencyResponse(nBands);
    }
}

Eigen::ArrayXXf IIRFilter::getFilter() const
{
    if (getCoefficients().nSos == 1)
    {
        return static_cast<Implementation<IIRFilter2ndOrder, IIRFilterConfiguration>*>(pimpl.get())->algo.getFilter();
    }
    else
    {
        return static_cast<Implementation<IIRFilterCascaded, IIRFilterConfiguration>*>(pimpl.get())->algo.getFilter();
    }
}

float IIRFilter::getGain() const
{
    if (getCoefficients().nSos == 1) { return 1.f;}
    else
    {
        return static_cast<Implementation<IIRFilterCascaded, IIRFilterConfiguration>*>(pimpl.get())->algo.getGain();
    }
}