#pragma once
#include "framework/framework.h"
#include "algorithm_library/noise_estimation.h"
#include "activity_detection/activity_detection_noise_estimation.h"

// noise estimation based on an activity detector. The more activity in the power spectra, the more the output is smoothed.
//
// author: Kristian Timm Andersen
class NoiseEstimationActivityDetection : public AlgorithmImplementation<NoiseEstimationConfiguration, NoiseEstimationActivityDetection>
{
public:
    NoiseEstimationActivityDetection(Coefficients c =  Coefficients()) : BaseAlgorithm{c},
        activityDetection(convertToActivityDetectionCoefficients(c))
    { 
        activity.resize(C.nBands, C.nChannels);
    }

    ActivityDetectionNoiseEstimation activityDetection;
    DEFINE_MEMBER_ALGORITHMS(activityDetection)

private:

    inline void processOn(Input powerNoisy, Output output)
    {
        activityDetection.process(powerNoisy, activity);
        output = activityDetection.getPowerNoise();
    }

    size_t getDynamicSizeVariables() const final
    {
        return activity.getDynamicMemorySize();
    }

    ActivityDetectionNoiseEstimation::Coefficients convertToActivityDetectionCoefficients(const Coefficients& c)
    {
        ActivityDetectionNoiseEstimation::Coefficients c2;
        c2.filterbankRate = c.filterbankRate;
        c2.nBands = c.nBands;
        c2.nChannels = c.nChannels;
        return c2;
    }

    Eigen::ArrayXXf activity;
    friend BaseAlgorithm;
};


// noise estimation based on an activity detector. The more activity in the power spectra, the more the output is smoothed. This version also has an activity detector output
//
// author: Kristian Timm Andersen
class NoiseEstimationOutputActivityDetection : public AlgorithmImplementation<NoiseEstimationActivityConfiguration, NoiseEstimationOutputActivityDetection>
{
public:
    NoiseEstimationOutputActivityDetection(Coefficients c =  Coefficients()) : BaseAlgorithm{c},
        activityDetection(convertToActivityDetectionCoefficients(c))
    { }

    ActivityDetectionNoiseEstimation activityDetection;
    DEFINE_MEMBER_ALGORITHMS(activityDetection)

private:

    inline void processOn(Input powerNoisy, Output output)
    {
        activityDetection.process(powerNoisy, output.activity);
        output.powerNoise = activityDetection.getPowerNoise();
    }

    ActivityDetectionNoiseEstimation::Coefficients convertToActivityDetectionCoefficients(const Coefficients& c)
    {
        ActivityDetectionNoiseEstimation::Coefficients c2;
        c2.filterbankRate = c.filterbankRate;
        c2.nBands = c.nBands;
        c2.nChannels = c.nChannels;
        return c2;
    }

    friend BaseAlgorithm;
};



// noise estimation based on an activity detector. The more activity in the power spectra, the more the output is smoothed. This version has an activity detector output as a single boolean value
//
// author: Kristian Timm Andersen
class NoiseEstimationOutputFusedActivityDetection : public AlgorithmImplementation<NoiseEstimationActivityFusedConfiguration, NoiseEstimationOutputFusedActivityDetection>
{
public:
    NoiseEstimationOutputFusedActivityDetection(Coefficients c =  Coefficients()) : BaseAlgorithm{c},
        activityDetection(convertToActivityDetectionCoefficients(c))
    { }

    ActivityDetectionFusedNoiseEstimation activityDetection;
    DEFINE_MEMBER_ALGORITHMS(activityDetection)

private:

    inline void processOn(Input powerNoisy, Output output)
    {
        activityDetection.process(powerNoisy, output.activity);
        output.powerNoise = activityDetection.getPowerNoise();
    }

    ActivityDetectionFusedNoiseEstimation::Coefficients convertToActivityDetectionCoefficients(const Coefficients& c)
    {
        ActivityDetectionFusedNoiseEstimation::Coefficients c2;
        c2.filterbankRate = c.filterbankRate;
        c2.nBands = c.nBands;
        c2.nChannels = c.nChannels;
        return c2;
    }

    friend BaseAlgorithm;
};
