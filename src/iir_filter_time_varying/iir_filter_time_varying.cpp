#include "iir_filter_time_varying/state_variable_filter.h"

// ------------------------------------------ IIRFilterTimeVarying and StateVariableFilter glue layer ----------------------------------------------------------------

DEFINE_ALGORITHM_CONSTRUCTOR(IIRFilterTimeVarying, StateVariableFilter, IIRFilterTimeVaryingConfiguration)

Eigen::ArrayXf IIRFilterTimeVarying::getSosFilter(float cutoff, float gain, float resonance) const
{
    return static_cast<Implementation<StateVariableFilter, IIRFilterTimeVaryingConfiguration> *>(pimpl.get())->algo.getSosFilter(cutoff, gain, resonance);
}

Eigen::ArrayXf IIRFilterTimeVarying::getPowerFrequencyResponse(int nBands, float cutoff, float gain, float resonance) const
{
    return static_cast<Implementation<StateVariableFilter, IIRFilterTimeVaryingConfiguration> *>(pimpl.get())->algo.getPowerFrequencyResponse(nBands, cutoff, gain, resonance);
}

Eigen::Array3f IIRFilterTimeVarying::setUserDefinedFilter(I::Real sos)
{
    return static_cast<Implementation<StateVariableFilter, IIRFilterTimeVaryingConfiguration> *>(pimpl.get())->algo.setUserDefinedFilter(sos);
}

// ------------------------------------------ IIRFilterCascadeTimeVarying and StateVariableFilterCascade glue layer
// ----------------------------------------------------------------

DEFINE_ALGORITHM_CONSTRUCTOR(IIRFilterCascadeTimeVarying, StateVariableFilterCascade, IIRFilterCascadeTimeVaryingConfiguration)

// set/get filter type of all filters in cascade
void IIRFilterCascadeTimeVarying::setFilterTypes(const std::vector<IIRFilterTimeVaryingConfiguration::Parameters::FilterTypes> &vec)
{
    static_cast<Implementation<StateVariableFilterCascade, IIRFilterCascadeTimeVaryingConfiguration> *>(pimpl.get())->algo.setFilterTypes(vec);
}

std::vector<IIRFilterTimeVaryingConfiguration::Parameters::FilterTypes> IIRFilterCascadeTimeVarying::getFilterTypes() const
{
    return static_cast<Implementation<StateVariableFilterCascade, IIRFilterCascadeTimeVaryingConfiguration> *>(pimpl.get())->algo.getFilterTypes();
}

// set/get filter type of one filter in cascade
void IIRFilterCascadeTimeVarying::setFilterType(int index, IIRFilterTimeVaryingConfiguration::Parameters::FilterTypes type)
{
    static_cast<Implementation<StateVariableFilterCascade, IIRFilterCascadeTimeVaryingConfiguration> *>(pimpl.get())->algo.setFilterType(index, type);
}

IIRFilterTimeVaryingConfiguration::Parameters::FilterTypes IIRFilterCascadeTimeVarying::getFilterType(int index) const
{
    return static_cast<Implementation<StateVariableFilterCascade, IIRFilterCascadeTimeVaryingConfiguration> *>(pimpl.get())->algo.getFilterType(index);
}

// get overall power frequency response  given filter characteristics (uses internal filter types and sample rate)
Eigen::ArrayXf IIRFilterCascadeTimeVarying::getPowerFrequencyResponse(int nBands, I::Real cutoffSos, I::Real gainSos, I::Real resonanceSos) const
{
    return static_cast<Implementation<StateVariableFilterCascade, IIRFilterCascadeTimeVaryingConfiguration> *>(pimpl.get())
        ->algo.getPowerFrequencyResponse(nBands, cutoffSos, gainSos, resonanceSos);
}

// get all sos filter coefficients given filter characteristics (uses internal filter types and sample rate)
Eigen::ArrayXXf IIRFilterCascadeTimeVarying::getSosFilter(I::Real cutoffSos, I::Real gainSos, I::Real resonanceSos) const
{
    return static_cast<Implementation<StateVariableFilterCascade, IIRFilterCascadeTimeVaryingConfiguration> *>(pimpl.get())
        ->algo.getSosFilter(cutoffSos, gainSos, resonanceSos);
}

// get overall gain
float IIRFilterCascadeTimeVarying::getGain() const
{
    return static_cast<Implementation<StateVariableFilterCascade, IIRFilterCascadeTimeVaryingConfiguration> *>(pimpl.get())->algo.getGain();
}

// set P.filterType to USER_DEFINED, return cutoff, resonance and gain, and set cLP, cBP and cHP for each second order section
Eigen::Array3Xf IIRFilterCascadeTimeVarying::setUserDefinedFilter(I::Real2D sos)
{
    return static_cast<Implementation<StateVariableFilterCascade, IIRFilterCascadeTimeVaryingConfiguration> *>(pimpl.get())->algo.setUserDefinedFilter(sos);
}