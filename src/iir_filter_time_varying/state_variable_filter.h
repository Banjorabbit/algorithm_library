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
        Eigen::ArrayXf g = (3.14159f * input.cutoff / C.sampleRate).unaryExpr(std::ref(fastertan));
		Eigen::ArrayXf c1 = g / input.resonance;
		Eigen::ArrayXf c2 = g * input.resonance;
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
                    output.col(i) = input.gain * lp.col(i);
                }
                break;
            case P.HighPass:
                for (auto i = 0; i < C.nChannels; i++)
                {
                    output.col(i) = input.gain * hp.col(i);
                }
                break;
            case P.BandPass:
                for (auto i = 0; i < C.nChannels; i++)
                {
                    output.col(i) = input.gain * bp.col(i);
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
                    output.col(i) = input.xTime.col(i) + input.gain * bp.col(i);
                }
                break;
            case P.LowShelf:
                for (auto i = 0; i < C.nChannels; i++)
                {
                    output.col(i) = input.gain * (bp.col(i) + hp.col(i)) + lp.col(i);
                }
                break;
            case P.HighShelf:
                for (auto i = 0; i < C.nChannels; i++)
                {
                    output.col(i) = input.gain * (bp.col(i) + lp.col(i)) + hp.col(i);
                }
                break;
        }
    }

    std::array<float,6> getDFCoefficients(float cutoff, float gain, float resonance) const
    {
        float g = fastertan(3.14159f * cutoff / C.sampleRate);
		float c1 = g / resonance;
		float c2 = g * resonance;
		float c3 = c2 + 1.f;
		float c0 = 1.f / (1.f + c1 * (c2 + 1.f));

        float a1 = 2*(c0*c1*c2 + c0*c1*c3 - 1);
        float a2 = 2*c0*c1*c2 - 2*c0*c1*c3 + 1;

        float b0,b1,b2;
        switch (P.filterType)
        {
            case P.LowPass:
                b0 = c0*c1*c2;
                b1 = 2*c0*c1*c2;
                b2 = c0*c1*c2;
                break;
            case P.HighPass:
                b0 = c0;
                b1 = -2*c0;
                b2 = c0;
                break;
            case P.BandPass:
                b0 = c0*c1;
                b1 = 0;
                b2 = -c0*c1;
                break;
            case P.BandStop:
                b0 = 1 - c0*c1;
                b1 = 2*(c0*c1*c2 + c0*c1*c3 - 1);
                b2 = 2*c0*c1*c2 - 2*c0*c1*c3 + c0*c1 + 1;
                break;
            case P.Peaking:
                b0 = 1 + c0*c1*gain;
                b1 = 2*(c0*c1*c2 + c0*c1*c3 - 1);
                b2 = 2*c0*c1*c2 - 2*c0*c1*c3 - c0*c1*gain + 1;
                break;
            case P.LowShelf:
                b0 = c0*(c1*c2 + gain*c1 + gain);
                b1 = 2*c0*(c1*c2 - gain);
                b2 = c0*(c1*c2 - gain*c1 + gain);
                break;
            case P.HighShelf:
                b0 = c0*(c1*c2*gain + c1*gain + c1);
                b1 = 2*c0*(c1*c2*gain - 1);
                b2 = c0*(c1*c2*gain - c1*gain + 1);
                break;
        }

        return {b0, b1, b2, 1.f, a1, a2};
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
class StateVariableFilterCascade : public AlgorithmImplementation<IIRFilterCascadeTimeVaryingConfiguration, StateVariableFilterCascade>
{
public:
    StateVariableFilterCascade(Coefficients c = Coefficients()) :
        AlgorithmImplementation<IIRFilterCascadeTimeVaryingConfiguration, StateVariableFilterCascade>{ c },
        filters(c.nSos, convertToStateVariableFilterCoefficients(c))
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

    void setFilterType(int index, StateVariableFilter::Parameters::FilterTypes type)
    {
        if (index < C.nSos && index >= 0) { filters[index].setParameters({type}); }
    }

    // get power frequency response evaluated uniformly from 0 to pi in nBands points
    Eigen::ArrayXf getPowerFrequencyResponse(int nBands)
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

    StateVariableFilter::Coefficients convertToStateVariableFilterCoefficients(const Coefficients& c)
    {
        StateVariableFilter::Coefficients cv;
        cv.nChannels = c.nChannels;
        cv.sampleRate = c.sampleRate;
        return cv;
    }

    float gain;
};