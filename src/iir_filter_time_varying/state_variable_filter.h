#pragma once
#include "algorithm_library/iir_filter_time_varying.h"
#include "framework/framework.h"

// State Variable Filter
//
// author: Kristian Timm Andersen
class StateVariableFilter : public AlgorithmImplementation<IIRFilterTimeVaryingConfiguration, StateVariableFilter>
{
  public:
    StateVariableFilter(const Coefficients &c = Coefficients()) : BaseAlgorithm{c}
    {
        z1.resize(c.nChannels);
        z2.resize(c.nChannels);
        cLP = 1.f;
        cBP = 1.f;
        cHP = 1.f;
        resetVariables();
    }

    Eigen::ArrayXf getSosFilter(float cutoff, float gain, float resonance) const
    {
        float c1 = cutoff / (resonance + 1e-16f); // add small dither to prevent division by zero
        float c2 = cutoff * resonance;
        float c3 = c2 + 1.f;
        float c0 = 1.f / (1.f + c1 * (c2 + 1.f));

        float b0{}, b1{}, b2{};
        const float c01 = c0 * c1;
        const float c012 = c01 * c2;
        const float c013 = c01 * c3;

        const float a1 = 2 * (c012 + c013 - 1);
        const float a2 = 2 * c012 - 2 * c013 + 1;

        switch (P.filterType)
        {
        case Parameters::LOWPASS:
            b0 = gain * c012;
            b1 = gain * 2 * c012;
            b2 = gain * c012;
            break;
        case Parameters::HIGHPASS:
            b0 = gain * c0;
            b1 = -gain * 2 * c0;
            b2 = gain * c0;
            break;
        case Parameters::BANDPASS:
            b0 = gain * c01;
            b1 = 0;
            b2 = -gain * c01;
            break;
        case Parameters::BANDSTOP:
            b0 = gain * (1 - c01);
            b1 = 2 * gain * (c012 + c013 - 1);
            b2 = gain * (2 * (c012 - c013) + c01 + 1);
            break;
        case Parameters::PEAKING:
            b0 = 1 + c01 * (gain - 1.f);
            b1 = 2 * (c012 + c013 - 1);
            b2 = 2 * c012 - 2 * c013 - c01 * (gain - 1.f) + 1;
            break;
        case Parameters::LOWSHELF:
            b0 = c012 + gain * (c01 + c0);
            b1 = 2 * (c012 - c0 * gain);
            b2 = (c012 - gain * (c01 - c0));
            break;
        case Parameters::HIGHSHELF:
            b0 = (gain * (c012 + c01) + c0);
            b1 = 2 * (c012 * gain - c0);
            b2 = (gain * (c012 - c01) + c0);
            break;
        case Parameters::USER_DEFINED:
            b0 = gain * (cLP * c012 + cBP * c01 + cHP * c0);
            b1 = 2 * gain * (cLP * c012 - cHP * c0);
            b2 = gain * (cLP * c012 - cBP * c01 + cHP * c0);
            break;
        }

        Eigen::ArrayXf c(6);
        c << b0, b1, b2, 1.f, a1, a2;
        return c;
    }

    // Given a second order section of the type:
    // sos = [b0, b1, b2, 1.f, a1, a2]
    // set P.filterType to USER_DEFINED, return cutoff, gain and resonance, and set cLP, cBP and cHP
    Eigen::Array3f setUserDefinedSosFilter(I::Real sos)
    {
        const float c45m = -sos(4) + sos(5) + 1;
        const float c45p = sos(4) + sos(5) + 1;
        const float cutoff = std::sqrt(-sos(4) * sos(4) + sos(5) * sos(5) + 2 * sos(5) + 1) / std::fabs(c45m);
        const float resonance = std::sqrt(c45m * c45p) / std::fabs(2 * (sos(5) - 1));
        cHP = (sos(0) - sos(1) + sos(2)) / c45m;
        cBP = (-sos(0) + sos(2)) / (sos(5) - 1);
        cLP = (sos(0) + sos(1) + sos(2)) / c45p;
        const float gain = (cHP + cBP + cLP) / 3.f;
        cHP /= gain;
        cBP /= gain;
        cLP /= gain;

        setParameters({P.USER_DEFINED});
        Eigen::Array3f cgr;
        cgr << cutoff, gain, resonance;
        return cgr;
    }

  private:
    inline void processAlgorithm(Input input, Output output)
    {
        for (auto sample = 0; sample < input.xTime.rows(); sample++)
        {
            const float c1 = input.cutoff(sample) / (input.resonance(sample) + 1e-16f); // add small dither to prevent division by zero
            const float c2 = input.cutoff(sample) * input.resonance(sample);
            const float c3 = c2 + 1.f;
            const float c0 = 1.f / (1.f + c1 * c3);
            for (auto channel = 0; channel < C.nChannels; channel++) // channel in inner loop is faster according to profiling
            {
                const float hp = c0 * (input.xTime(sample, channel) - z2(channel) - c3 * z1(channel));
                const float x1 = c1 * hp;
                const float bp = x1 + z1(channel);
                const float x2 = c2 * bp;
                const float lp = x2 + z2(channel);

                z1(channel) = x1 + bp;
                z2(channel) = x2 + lp;

                switch (P.filterType)
                {
                case Parameters::LOWPASS: output(sample, channel) = input.gain(sample) * lp; break;
                case Parameters::HIGHPASS: output(sample, channel) = input.gain(sample) * hp; break;
                case Parameters::BANDPASS: output(sample, channel) = input.gain(sample) * bp; break;
                case Parameters::BANDSTOP: output(sample, channel) = input.gain(sample) * (lp + hp); break;
                case Parameters::PEAKING: output(sample, channel) = lp + hp + input.gain(sample) * bp; break;
                case Parameters::LOWSHELF: output(sample, channel) = input.gain(sample) * (bp + hp) + lp; break;
                case Parameters::HIGHSHELF: output(sample, channel) = input.gain(sample) * (bp + lp) + hp; break;
                case Parameters::USER_DEFINED: output(sample, channel) = input.gain(sample) * (cLP * lp + cBP * bp + cHP * hp); break;
                }
            }
        }
    }

    void resetVariables() final
    {
        z1.setZero();
        z2.setZero();
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = z1.getDynamicMemorySize();
        size += z2.getDynamicMemorySize();
        return size;
    }

    Eigen::ArrayXf z1;
    Eigen::ArrayXf z2;
    float cLP, cBP, cHP;

    friend BaseAlgorithm;
};

// Cascade of StateVariableFilter
class StateVariableFilterCascade : public AlgorithmImplementation<IIRFilterCascadeTimeVaryingConfiguration, StateVariableFilterCascade>
{
  public:
    StateVariableFilterCascade(Coefficients c = Coefficients()) : BaseAlgorithm{c}, filters(c.nSos, {c.nChannels}) { gain = 1.f; }

    VectorAlgo<StateVariableFilter> filters;
    DEFINE_MEMBER_ALGORITHMS(filters)

    void setFilterTypes(const std::vector<StateVariableFilter::Parameters::FilterTypes> &vec)
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

    StateVariableFilter::Parameters::FilterTypes getFilterType(int index) const { return filters[index].getParameters().filterType; }

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
    Eigen::Array3Xf setUserDefinedSosFilter(I::Real2D sos)
    {
        Eigen::Array3Xf cgr(3, C.nSos);
        for (auto i = 0; i < C.nSos; i++)
        {
            cgr.col(i) = filters[i].setUserDefinedSosFilter(sos.col(i));
        }
        return cgr;
    }

  private:
    inline void processAlgorithm(Input input, Output output)
    {
        output = input.xTime * gain;
        for (auto i = 0; i < C.nSos; i++)
        {
            filters[i].process({output, input.cutoff.col(i), input.gain.col(i), input.resonance.col(i)}, output);
        }
    }

    float gain;

    friend BaseAlgorithm;
};