#pragma once
#include "framework/framework.h"
#include "algorithm_library/iir_filter.h"

// 2nd order Direct Form II Transposed IIR filter
//
// author: Kristian Timm Andersen

class IIRFilter2ndOrder : public AlgorithmImplementation<IIRFilterConfiguration, IIRFilter2ndOrder>
{
public:
    IIRFilter2ndOrder(Coefficients c = Coefficients()) :
        AlgorithmImplementation<IIRFilterConfiguration, IIRFilter2ndOrder>{ c }
    { 
        C.nSos = 1; // overwrite in case another value is passed on
        state1.resize(c.nChannels);
        state2.resize(c.nChannels);

        resetVariables();
        a1 = 0.f;
        a2 = 0.f;
        b0 = 1.f;
        b1 = 0.f;
        b2 = 0.f;
    }

    inline void processOn(Input input, Output output)
    {
        for (auto sample = 0; sample < input.rows(); sample++)
		{
			for (auto channel = 0; channel < C.nChannels; channel++) // channel in inner loop is faster according to profiling
			{
				float out = input(sample, channel) * b0 + state1(channel); // can not write directly to output if input and output are same memory
				// Update state variables
				state1(channel) = input(sample, channel) * b1 - out * a1 + state2(channel);
				state2(channel) = input(sample, channel) * b2 - out * a2;
				output(sample, channel) = out;
			}
		}
    }

    void setFilter(I::Real sos)
    {
        if (sos(3) != 0.f)
        {
            b0 = sos(0) / sos(3);
            b1 = sos(1) / sos(3);
            b2 = sos(2) / sos(3);
            a1 = sos(4) / sos(3);
            a2 = sos(5) / sos(3);
        }
        else // this is not a valid filter so set all filter coefficients to zero
        {
            b0 = 0.f;
            b1 = 0.f;
            b2 = 0.f;
            a1 = 0.f;
            a2 = 0.f;
        }
    }

    // get power frequency response evaluated uniformly from 0 to pi in nBands points
    Eigen::ArrayXf getPowerFrequencyReponse(int nBands) const
    {
        Eigen::ArrayXf freqs = Eigen::ArrayXf::LinSpaced(nBands, 0, 3.14159);
        return (b0*b0 + b1*b1 + b2*b2 + 2*(b0*b1+b1*b2)*freqs.cos() + 2*b0*b2*(2*freqs).cos()) / (1.f + a1*a1 + a2*a2 + 2*(a1+a1*a2)*freqs.cos() + 2*a2*(2*freqs).cos());
    }

    Eigen::ArrayXXf getFilter() const
    {
        Eigen::ArrayXXf sos(1,6);
        sos << b0, b1, b2, 1.f, a1, a2;
        return sos;
    }

private:

    void resetVariables() final
    {
        state1.setZero();
        state2.setZero();
    }

    size_t getDynamicSizeVariables() const final
    { 
        size_t size = state1.getDynamicMemorySize();
        size += state2.getDynamicMemorySize();
        return size;
    }

    float b0, b1, b2, a1, a2; // coefficients
    Eigen::ArrayXf state1, state2; // state variables
};

// Cascade of IIRFilter2ndOrder
class IIRFilterCascaded : public AlgorithmImplementation<IIRFilterConfiguration, IIRFilterCascaded>
{
public:
    IIRFilterCascaded(Coefficients c = Coefficients()) :
        AlgorithmImplementation<IIRFilterConfiguration, IIRFilterCascaded>{ c },
        filters(c.nSos,c)
    { gain = 1.f; }

    VectorAlgo<IIRFilter2ndOrder> filters;
    DEFINE_MEMBER_ALGORITHMS(filters)

    inline void processOn(Input input, Output output)
    {
        output = input * gain;
		for (auto i = 0; i < C.nSos; i++) { filters[i].process(output, output); }
    }

    void setFilter(I::Real2D sos, float g)
    {
        gain = g;
        for (auto i = 0; i < C.nSos; i++)
        {
            filters[i].setFilter(sos.row(i).transpose());
        }
    }

    // get power frequency response evaluated uniformly from 0 to pi in nBands points
    Eigen::ArrayXf getPowerFrequencyReponse(int nBands)
    {
        Eigen::ArrayXf response = Eigen::ArrayXf::Ones(nBands);
        for (auto i = 0; i < C.nSos; i++)
        {
            response *= filters[i].getPowerFrequencyReponse(nBands);
        }
        return response;
    }

    Eigen::ArrayXXf getFilter() const
    {
        Eigen::ArrayXXf sos(C.nSos, 6);
        for (auto i = 0; i < C.nSos; i++)
        {
            sos.row(i) = filters[i].getFilter();
        }
        return sos;
    }

    float getGain() const { return gain; }

private:
    float gain;
};