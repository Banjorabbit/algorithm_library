#pragma once
#include "framework/framework.h"
#include "algorithm_library/noise_estimation.h"
#include "utilities/functions.h"

// noise estimation based on an activity detector. The more activity in the power spectra, the more the output is smoothed.
//
// author: Kristian Timm Andersen
class NoiseEstimationActivityDetection : public AlgorithmImplementation<NoiseEstimationConfiguration, NoiseEstimationActivityDetection>
{
public:
    NoiseEstimationActivityDetection(Coefficients c =  Coefficients()) :
        AlgorithmImplementation<NoiseEstimationConfiguration, NoiseEstimationActivityDetection>{ c }
    {
        activityMean.resize(c.nBands, c.nChannels);
        powerNoise.resize(c.nBands, c.nChannels);
        activityMeanLambda = 1.f - expf(-1.f / (c.filterbankRate * .152f));
        resetVariables();
        onParametersChanged();
     }

    inline void processOn(Input input, Output output)
    {
        // activity detection
        output.activity = (0.9693465699682844f * input / (powerNoise + 1e-20f) - 3.485010713180571f).cwiseMin(25.f);

        // for-loop has been profiled to be faster in gcc than the commented line below:
        // output.activity = output.activity.unaryExpr(std::ref(fasterExp));
        float* ptr = output.activity.data();
        for (auto i = 0; i < output.activity.size(); i++, ptr++)
        {
            *ptr = fasterExp(*ptr);
        }
        
        output.activity /= (1.f + output.activity);

        // smooth activity
        activityMean += activityMeanLambda * (output.activity - activityMean);
        output.activity = (activityMean > 0.99f).select(output.activity.cwiseMin(.99f), output.activity);

        // update noise
        powerNoise += smoothingLambda * (1.f - output.activity) * (input - powerNoise);
        output.powerNoise = powerNoise;
    }

    void onParametersChanged()
    {
        smoothingLambda = 1.f - expf(-1.f / (C.filterbankRate * P.smoothingTConstant));
    } 

private:

    void resetVariables() final
    {
        activityMean.setZero();
        powerNoise.setZero();
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = activityMean.getDynamicMemorySize();
        size += powerNoise.getDynamicMemorySize();
        return size;
    }

    Eigen::ArrayXXf activityMean;
    Eigen::ArrayXXf powerNoise;
    float activityMeanLambda, smoothingLambda;
};
