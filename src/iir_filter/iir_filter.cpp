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
// [b0  b1  b2  a0  a1  a2
//  bb0 bb1 bb2 aa0 aa1 aa2
//  ... ... ... ... ... ...]
void IIRFilter::setFilter(I::Real2D sos, float gain)
{
    if (getCoefficients().nSos == 1)
    {
        Eigen::ArrayXf sos1 = sos.row(0).transpose(); // filter only has 1 sos
        sos1.head(3) *= gain; // embed gain into sos
        setFilter(sos1);
    }
    else
    {
        static_cast<Implementation<IIRFilterCascaded, IIRFilterConfiguration>*>(pimpl.get())->algo.setFilter(sos, gain);
    }
}

// This method can be used when there is only one second-order-section. if nSos>1 then this method has no effect.
void IIRFilter::setFilter(I::Real sos) 
{
    if (getCoefficients().nSos == 1)
    {
        static_cast<Implementation<IIRFilter2ndOrder, IIRFilterConfiguration>*>(pimpl.get())->algo.setFilter(sos);
    }
}

// get power frequency response evaluated uniformly from 0 to pi in nBands points
Eigen::ArrayXf IIRFilter::getPowerFrequencyReponse(int nBands)
{
    if (getCoefficients().nSos == 1)
    {
        return static_cast<Implementation<IIRFilter2ndOrder, IIRFilterConfiguration>*>(pimpl.get())->algo.getPowerFrequencyReponse(nBands);
    }
    else
    {
        return static_cast<Implementation<IIRFilterCascaded, IIRFilterConfiguration>*>(pimpl.get())->algo.getPowerFrequencyReponse(nBands);
    }
}