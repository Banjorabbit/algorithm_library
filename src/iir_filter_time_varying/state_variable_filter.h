#pragma once
#include "framework/framework.h"
#include "algorithm_library/iir_filter_time_varying.h"
#include "utilities/fastonebigheader.h"
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
        Eigen::ArrayXf g = (3.14159f * input.cutoff.col(0) / C.sampleRate).unaryExpr(std::ref(fastertan));
		Eigen::ArrayXf c1 = g / P.resonance;
		Eigen::ArrayXf c2 = g * P.resonance;
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
                for (auto i = 0; i < C.nChannels; i++)
                {
                    output.col(i) = input.xTime.col(i) + input.gain.col(0) * bp.col(i);
                }
                break;
            case P.LowShelf:
                for (auto i = 0; i < C.nChannels; i++)
                {
                    output.col(i) = input.gain.col(0) * (bp.col(i) + hp.col(i)) + lp.col(i);
                }
                break;
            case P.HighShelf:
                for (auto i = 0; i < C.nChannels; i++)
                {
                    output.col(i) = input.gain.col(0) * (bp.col(i) + lp.col(i)) + hp.col(i);
                }
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