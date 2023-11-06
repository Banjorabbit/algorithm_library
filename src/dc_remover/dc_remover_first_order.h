#pragma once
#include "framework/framework.h"
#include "algorithm_library/dc_remover.h"

using namespace Eigen;

class DCRemoverFirstOrder : public AlgorithmImplementation<DCRemoverConfiguration, DCRemoverFirstOrder>
{
public:
    DCRemoverFirstOrder(Coefficients c = Coefficients()) :
        AlgorithmImplementation<DCRemoverConfiguration, DCRemoverFirstOrder>{ c }
    {
        delay.resize(c.nChannels);
        resetVariables();
        onParametersChanged();
    }

    void processOn(Input input, Output output)
    {
        for (auto channel = 0; channel < C.nChannels; channel++)
        {
            for (auto sample = 0; sample < input.rows(); sample++)
            {
                const float xi = coef1 * input(sample,channel);
                output(sample, channel) = xi + delay(channel);
                delay(channel) = coef0 * output(sample, channel) - xi;
            }
        }
    }

    void onParametersChanged() 
    {
        const float cosf = std::cos(2*DCRemoverConfiguration::PI * P.cutoffFrequency / C.sampleRate);
        const float a = (1.f - std::sqrt(1.f - cosf*cosf))/cosf;

        coef0 = a;
        coef1 = (1+a)/2;
    }

private:

    void resetVariables() final 
    {
        delay.setZero();
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = delay.getDynamicMemorySize();
        return size;
    }

    ArrayXf delay;
    float coef0, coef1;
};