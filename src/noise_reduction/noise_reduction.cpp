#include "noise_reduction/noise_reduction_apriori.h"
#include "noise_reduction/noise_reduction_ml.h"

using AprioriImpl = Implementation<NoiseReductionAPriori, NoiseReductionConfiguration>;
using MLImpl = Implementation<NoiseReductionML, NoiseReductionConfiguration>;

template <>
Algorithm<NoiseReductionConfiguration>::Algorithm(const Coefficients &c)
{
    if (c.algorithmType == c.APRIORI) { pimpl = std::make_unique<AprioriImpl>(c); }
    else { pimpl = std::make_unique<MLImpl>(c); }
}

NoiseReduction::NoiseReduction(const Coefficients &c) : Algorithm<NoiseReductionConfiguration>(c) {}