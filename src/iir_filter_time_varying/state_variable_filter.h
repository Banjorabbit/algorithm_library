#pragma once
#include "framework/framework.h"
#include "algorithm_library/iir_filter_time_varying.h"

class StateVariableFilter : public AlgorithmImplementation<IIRFilterTimeVaryingConfiguration, StateVariableFilter>
{
public:
    StateVariableFilter(const Coefficients& c = Coefficients())
        : AlgorithmImplementation<IIRFilterTimeVaryingConfiguration, StateVariableFilter>{ c }
    {
        z1.resize(c.nChannels);
        z2.resize(c.nChannels);
        resetVariables();
    }
    
    inline void processOn(Input input, Output output) 
    {
        Eigen::ArrayXf c = (3.14159f * input.cutoff.col(0) / C.sampleRate).tan();
        Eigen::ArrayXf cd = c * P.resonance;
        Eigen::ArrayXf cdc = cd * c;
        Eigen::ArrayXf a0 = cdc / (cdc + c + cd);
        Eigen::ArrayXf a1 = 2.f * a0;
        Eigen::ArrayXXf hp(input.xTime.rows(), C.nChannels), bp(input.xTime.rows(), C.nChannels), lp(input.xTime.rows(), C.nChannels);

        for (auto sample = 0; sample < input.xTime.rows(); sample++)
        {
            for (auto channel = 0; channel < C.nChannels; channel++) // channel in inner loop is faster according to profiling
            {
                hp(sample, channel) = (input.xTime(sample, channel) - z1(channel) - 2.0f * z2(channel)) * a0(sample);
                bp(sample, channel) = hp(sample, channel) * a1(sample) + z1(channel);
                lp(sample, channel) = bp(sample, channel) * a1(sample) + z2(channel);

                z1(channel) = hp(sample, channel) * a1(sample) + bp(sample, channel);
                z2(channel) = hp(sample, channel) * a0(sample) + lp(sample, channel);
            }
        }
        switch (P.filterType)
        {
            case P.LowPass:
                output = lp;
                break;
            case P.HighPass:
                output = hp;
                break;
            case P.BandPass:
                output = bp;
                break;
            case P.BandStop:
                output = input.xTime - bp;
                break;
            case P.Peaking:
                output = input.xTime + input.gain.col(0).replicate(1,C.nChannels) * bp;
                break;
            case P.LowShelf:            
                output = input.gain.col(0).replicate(1,C.nChannels) * (bp + hp) + lp;
                break;
            case P.HighShelf:
                output = input.gain.col(0).replicate(1,C.nChannels) * (bp + lp) + hp;
                break;
        }
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
};