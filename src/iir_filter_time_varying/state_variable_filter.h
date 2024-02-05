#pragma once
#include "framework/framework.h"
#include "algorithm_library/iir_filter_time_varying.h"

// State Variable Filter 
//
// author: Kristian Timm Andersen
class StateVariableFilter : public AlgorithmImplementation<IIRFilterTimeVaryingConfiguration, StateVariableFilter>
{
public:
    StateVariableFilter(const Coefficients& c = Coefficients())
        : AlgorithmImplementation<IIRFilterTimeVaryingConfiguration, StateVariableFilter>{ c }
    {
        z1.resize(c.nChannels);
        z2.resize(c.nChannels);
        resetVariables();
        cLP = 1.f;
        cBP = 1.f;
        cHP = 1.f;
    }
    
    inline void processOn(Input input, Output output) 
    {
		Eigen::ArrayXf c1 = input.cutoff / input.resonance;
		Eigen::ArrayXf c2 = input.cutoff * input.resonance;
		Eigen::ArrayXf c3 = c2 + 1.f;
		Eigen::ArrayXf c0 = 1.f / (1.f + c1 * (c2 + 1.f));

        Eigen::ArrayXXf hp(input.xTime.rows(), C.nChannels), bp(input.xTime.rows(), C.nChannels), lp(input.xTime.rows(), C.nChannels);

        for (auto sample = 0; sample < input.xTime.rows(); sample++)
        {
            for (auto channel = 0; channel < C.nChannels; channel++) // channel in inner loop is faster according to profiling
            {
                hp(sample, channel) = c0(sample) * (input.xTime(sample, channel) - z2(channel) - c3(sample) * z1(channel));
				const auto x1 = c1(sample) * hp(sample, channel);
				bp(sample, channel) = x1 + z1(channel);
				const auto x2 = c2(sample) * bp(sample, channel);
				lp(sample, channel) = x2 + z2(channel);

				z1(channel) = x1 + bp(sample, channel);
				z2(channel) = x2 + lp(sample, channel);
            }
        }
        
        switch (P.filterType)
        {
        case P.LOWPASS:
            for (auto i = 0; i < C.nChannels; i++)
            {
                output.col(i) = input.gain * lp.col(i);
            }
            break;
        case P.HIGHPASS:
            for (auto i = 0; i < C.nChannels; i++)
            {
                output.col(i) = input.gain * hp.col(i);
            }
            break;
        case P.BANDPASS:
            for (auto i = 0; i < C.nChannels; i++)
            {
                output.col(i) = input.gain * bp.col(i);
            }
            break;
        case P.BANDSTOP:
            for (auto i = 0; i < C.nChannels; i++)
            {
                output.col(i) = input.gain * (input.xTime.col(i) - bp.col(i));
            }
            break;
        case P.PEAKING:
            for (auto i = 0; i < C.nChannels; i++)
            {
                output.col(i) = input.xTime.col(i) + (input.gain - 1.f) * bp.col(i);
            }
            break;
        case P.LOWSHELF:
            for (auto i = 0; i < C.nChannels; i++)
            {
                output.col(i) = input.gain * (bp.col(i) + hp.col(i)) + lp.col(i);
            }
            break;
        case P.HIGHSHELF:
            for (auto i = 0; i < C.nChannels; i++)
            {
                output.col(i) = input.gain * (bp.col(i) + lp.col(i)) + hp.col(i);
            }
            break;
        case P.USER_DEFINED:
            for (auto i = 0; i < C.nChannels; i++)
            {
                output.col(i) = input.gain * (cLP * lp.col(i) + cBP * bp.col(i) + cHP * hp.col(i));
            }
            break;
        }
    }

    Eigen::ArrayXf getSosFilter(float cutoff, float gain, float resonance) const
    {
		float c1 = cutoff / resonance;
		float c2 = cutoff * resonance;
		float c3 = c2 + 1.f;
		float c0 = 1.f / (1.f + c1 * (c2 + 1.f));

        float b0{},b1{},b2{};
        const float c01 = c0 * c1;
        const float c012 = c01 * c2;
        const float c013 = c01 * c3;

        const float a1 = 2 * (c012 + c013 - 1);
        const float a2 = 2 * c012 - 2 * c013 + 1;

        switch (P.filterType)
        {
        case P.LOWPASS:
            b0 = gain * c012;
            b1 = gain * 2 * c012;
            b2 = gain * c012;
            break;
        case P.HIGHPASS:
            b0 = gain * c0;
            b1 = -gain * 2 * c0;
            b2 = gain * c0;
            break;
        case P.BANDPASS:
            b0 = gain * c01;
            b1 = 0;
            b2 = -gain * c01;
            break;
        case P.BANDSTOP:
            b0 = gain * (1 - c01);
            b1 = 2 * gain * (c012 + c013 - 1);
            b2 = gain * (2 * (c012 - c013) + c01 + 1);
            break;
        case P.PEAKING:
            b0 = 1 + c01 * (gain - 1.f);
            b1 = 2 * (c012 + c013 - 1);
            b2 = 2 * c012 - 2 * c013 - c01 * (gain - 1.f) + 1;
            break;
        case P.LOWSHELF:
            b0 = c012 + gain * (c01 + c0);
            b1 = 2 * (c012 - c0 * gain);
            b2 = (c012 - gain * (c01 - c0));
            break;
        case P.HIGHSHELF:
            b0 = (gain * (c012 + c01) + c0);
            b1 = 2 * (c012 * gain - c0);
            b2 = (gain * (c012 - c01) + c0);
            break;
        case P.USER_DEFINED:
            b0 = gain * (cLP * c012 + cBP * c01 + cHP * c0);
            b1 = 2 * gain * (cLP * c012 - cHP * c0);
            b2 = gain * (cLP * c012 - cBP * c01 + cHP * c0);
            break;
        }

        Eigen::ArrayXf c(6);
        c << b0, b1, b2, 1.f, a1, a2;
        return c;
    }

        // get power frequency response evaluated uniformly from 0 to pi in nBands points
    Eigen::ArrayXf getPowerFrequencyResponse(int nBands, float cutoff, float gain, float resonance) const
    {
        Eigen::ArrayXf c = getSosFilter(cutoff, gain, resonance);
        const float b0 = c(0); 
        const float b1 = c(1);
        const float b2 = c(2);
        const float a1 = c(4);
        const float a2 = c(5);
        Eigen::ArrayXf freqs = Eigen::ArrayXf::LinSpaced(nBands, 0, 3.14159f);
        return (b0*b0 + b1*b1 + b2*b2 + 2*(b0*b1+b1*b2)*freqs.cos() + 2*b0*b2*(2*freqs).cos()) / (1.f + a1*a1 + a2*a2 + 2*(a1+a1*a2)*freqs.cos() + 2*a2*(2*freqs).cos());
    }

    // Given a second order section of the type:
    // sos = [b0, b1, b2, 1.f, a1, a2]
    // set P.filterType to USER_DEFINED, return cutoff, gain and resonance, and set cLP, cBP and cHP
    Eigen::Array3f setUserDefinedFilter (I::Real sos)
    {
        const float c45m = sos(4) - sos(5) - 1;
        const float c45p = sos(4) + sos(5) + 1;
        const float cutoff = std::sqrt(-sos(4)*sos(4) + sos(5)*sos(5) + 2*sos(5) + 1)/c45m;
        const float resonance = std::sqrt(-c45m*c45p)/(2*(sos(5) - 1)); 
        cHP = -(sos(0) - sos(1) + sos(2))/c45m;
        cBP = (-sos(0) + sos(2))/(sos(5) - 1);
        cLP = (sos(0) + sos(1) + sos(2))/c45p;

        setParameters({P.USER_DEFINED});
        Eigen::Array3f cgr;
        cgr << cutoff, 1.f, resonance;
        return cgr;
    }
    
private:

    void resetVariables() final
    {
        z1.setZero();
        z2.setZero();
    }

    size_t getDynamicSizeVariables() const final
    {
        return z1.getDynamicMemorySize() + z2.getDynamicMemorySize();
    }

    Eigen::ArrayXf z1;
    Eigen::ArrayXf z2;
    float cLP, cBP, cHP;
};


// Cascade of StateVariableFilter
// TODO: This algorithm is not finished!
class StateVariableFilterCascade : public AlgorithmImplementation<IIRFilterCascadeTimeVaryingConfiguration, StateVariableFilterCascade>
{
public:
    StateVariableFilterCascade(Coefficients c = Coefficients()) :
        AlgorithmImplementation<IIRFilterCascadeTimeVaryingConfiguration, StateVariableFilterCascade>{ c },
        filters(c.nSos, {c.nChannels})
    { gain = 1.f; }

    VectorAlgo<StateVariableFilter> filters;
    DEFINE_MEMBER_ALGORITHMS(filters)

    inline void processOn(Input input, Output output)
    {
        output = input.xTime * gain;
		for (auto i = 0; i < C.nSos; i++) 
        { 
            filters[i].process({output, input.cutoff.col(i), input.gain.col(i), input.resonance.col(i)}, output); 
        }
    }

    void setFilterTypes(const std::vector<StateVariableFilter::Parameters::FilterTypes>& vec)
    {
        if (C.nSos == static_cast<int>(vec.size()))
        {
            for (auto i = 0; i < C.nSos; i++)
            {
                filters[i].setParameters({vec[i]});
            }
        }
    }

    std::vector<StateVariableFilter::Parameters::FilterTypes> getFilterTypes() const 
    {
        std::vector<StateVariableFilter::Parameters::FilterTypes> filterTypes(C.nSos);
        for (auto i = 0; i < C.nSos; i++)
        {
            filterTypes[i] = filters[i].getParameters().filterType;
        }
        return filterTypes;
    }

    void setFilterType(int index, StateVariableFilter::Parameters::FilterTypes type)
    {
        if (index < C.nSos && index >= 0) { filters[index].setParameters({type}); }
    }

    StateVariableFilter::Parameters::FilterTypes getFilterType(int index) const 
    {
        return filters[index].getParameters().filterType;
    }

    // get power frequency response evaluated uniformly from 0 to pi in nBands points
    Eigen::ArrayXf getPowerFrequencyResponse(int nBands, I::Real cutoffSos, I::Real gainSos, I::Real resonanceSos) const
    {
        Eigen::ArrayXf response = Eigen::ArrayXf::Constant(nBands, gain);
        for (auto i = 0; i < C.nSos; i++)
        {
            response *= filters[i].getPowerFrequencyResponse(nBands, cutoffSos(i), gainSos(i), resonanceSos(i));
        }
        return response;
    }

    Eigen::ArrayXXf getSosFilter(I::Real cutoffSos, I::Real gainSos, I::Real resonanceSos) const
    {
        Eigen::ArrayXXf sos(6, C.nSos);
        for (auto i = 0; i < C.nSos; i++)
        {
            sos.col(i) = filters[i].getSosFilter(cutoffSos(i), gainSos(i), resonanceSos(i));
        }
        return sos;
    }

    float getGain() const { return gain; }
    void setGain(float g) { gain = g; }

    // Given a second order section of the type:
    // [b0 bb0 ... ]
    // [b1 bb1 ... ]
    // [b2 bb2 ... ]
    // [1  1   ... ]
    // [a1 aa1 ... ]
    // [a2 aa2 ... ]
    // set P.filterType to USER_DEFINED, return cutoff, resonance and gain, and set cLP, cBP and cHP for each second order section
    Eigen::Array3Xf setUserDefinedFilter (I::Real2D sos)
    {
        Eigen::Array3Xf cgr(3, C.nSos);
        for (auto i = 0; i < C.nSos; i++)
        {
            cgr.col(i) = filters[i].setUserDefinedFilter(sos.col(i));
        }
        return cgr;
    }
   
private:
    float gain;
};