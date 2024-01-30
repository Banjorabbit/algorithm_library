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
                for (auto i = 0; i < C.nChannels; i++)
                {
                    output.col(i) = input.gain.col(0) * lp.col(i);
                }
                break;
            case P.HighPass:
                for (auto i = 0; i < C.nChannels; i++)
                {
                    output.col(i) = input.gain.col(0) * hp.col(i);
                }
                break;
            case P.BandPass:
                for (auto i = 0; i < C.nChannels; i++)
                {
                    output.col(i) = input.gain.col(0) * bp.col(i);
                }
                break;
            case P.BandStop:
                for (auto i = 0; i < C.nChannels; i++)
                {
                    output.col(i) = input.xTime.col(i) - bp.col(i);
                }
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


// Cascade of StateVariableFilter
// TODO: This algorithm is not finished!
class StateVariableFilterCascaded : public AlgorithmImplementation<IIRFilterTimeVaryingConfiguration, StateVariableFilterCascaded>
{
public:
    StateVariableFilterCascaded(Coefficients c = Coefficients()) :
        AlgorithmImplementation<IIRFilterTimeVaryingConfiguration, StateVariableFilterCascaded>{ c },
        filters(c.nSos,c)
    { gain = 1.f; }

    VectorAlgo<StateVariableFilter> filters;
    DEFINE_MEMBER_ALGORITHMS(filters)

    inline void processOn(Input input, Output output)
    {
        output = input.xTime * gain;
		for (auto i = 0; i < C.nSos; i++) 
        { 
            filters[i].process({output, input.cutoff.col(i), input.gain.col(i)}, output); 
        }
    }

    void setFilter(I::Real2D sos, float g)
    {
        gain = g;
        for (auto i = 0; i < C.nSos; i++)
        {
            // filters[i].setFilter(sos.row(i).transpose());
        }
    }

    // get power frequency response evaluated uniformly from 0 to pi in nBands points
    Eigen::ArrayXf getPowerFrequencyReponse(int nBands)
    {
        Eigen::ArrayXf response = Eigen::ArrayXf::Ones(nBands);
        for (auto i = 0; i < C.nSos; i++)
        {
            // response *= filters[i].getPowerFrequencyReponse(nBands);
        }
        return response;
    }

    // Eigen::ArrayXXf getFilter() const
    // {
    //     Eigen::ArrayXXf sos(C.nSos, 6);
    //     for (auto i = 0; i < C.nSos; i++)
    //     {
    //         sos.row(i) = filters[i].getFilter();
    //     }
    //     return sos;
    // }

    float getGain() const { return gain; }

private:
    float gain;
};