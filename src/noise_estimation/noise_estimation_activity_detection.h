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
    NoiseEstimationActivityDetection(Coefficients c =  Coefficients()) :
        AlgorithmImplementation<NoiseEstimationConfiguration, NoiseEstimationActivityDetection>{ c },
        activityDetection(convertToActivityDetectionCoefficients(c))
    { }

    ActivityDetectionNoiseEstimation activityDetection;
    DEFINE_MEMBER_ALGORITHMS(activityDetection)

    inline void processOn(Input powerNoisy, Output output)
    {
        Eigen::ArrayXXf activity(C.nBands, C.nChannels);
        activityDetection.processOn(powerNoisy, activity);
        output = activityDetection.getPowerNoise();
    }

private:

    ActivityDetectionNoiseEstimation::Coefficients convertToActivityDetectionCoefficients(const Coefficients& c)
    {
        ActivityDetectionNoiseEstimation::Coefficients c2;
        c2.filterbankRate = c.filterbankRate;
        c2.nBands = c.nBands;
        c2.nChannels = c.nChannels;
        return c2;
    }
};


// noise estimation based on an activity detector. The more activity in the power spectra, the more the output is smoothed. This version also has an activity detector output
//
// author: Kristian Timm Andersen
class NoiseEstimationOutputActivityDetection : public AlgorithmImplementation<NoiseEstimationActivityConfiguration, NoiseEstimationOutputActivityDetection>
{
public:
    NoiseEstimationOutputActivityDetection(Coefficients c =  Coefficients()) :
        AlgorithmImplementation<NoiseEstimationActivityConfiguration, NoiseEstimationOutputActivityDetection>{ c },
        activityDetection(convertToActivityDetectionCoefficients(c))
    { }

    ActivityDetectionNoiseEstimation activityDetection;
    DEFINE_MEMBER_ALGORITHMS(activityDetection)

    inline void processOn(Input powerNoisy, Output output)
    {
        activityDetection.processOn(powerNoisy, output.activity);
        output.powerNoise = activityDetection.getPowerNoise();
    }

private:

    ActivityDetectionNoiseEstimation::Coefficients convertToActivityDetectionCoefficients(const Coefficients& c)
    {
        ActivityDetectionNoiseEstimation::Coefficients c2;
        c2.filterbankRate = c.filterbankRate;
        c2.nBands = c.nBands;
        c2.nChannels = c.nChannels;
        return c2;
    }
};



// noise estimation based on an activity detector. The more activity in the power spectra, the more the output is smoothed. This version has an activity detector output as a single boolean value
//
// author: Kristian Timm Andersen
class NoiseEstimationOutputFusedActivityDetection : public AlgorithmImplementation<NoiseEstimationActivityFusedConfiguration, NoiseEstimationOutputFusedActivityDetection>
{
public:
    NoiseEstimationOutputFusedActivityDetection(Coefficients c =  Coefficients()) :
        AlgorithmImplementation<NoiseEstimationActivityFusedConfiguration, NoiseEstimationOutputFusedActivityDetection>{ c },
        activityDetection(convertToActivityDetectionCoefficients(c))
    { }

    ActivityDetectionFusedNoiseEstimation activityDetection;
    DEFINE_MEMBER_ALGORITHMS(activityDetection)

    inline void processOn(Input powerNoisy, Output output)
    {
        activityDetection.processOn(powerNoisy, output.activity);
        output.powerNoise = activityDetection.getPowerNoise();
    }

private:

    ActivityDetectionFusedNoiseEstimation::Coefficients convertToActivityDetectionCoefficients(const Coefficients& c)
    {
        ActivityDetectionFusedNoiseEstimation::Coefficients c2;
        c2.filterbankRate = c.filterbankRate;
        c2.nBands = c.nBands;
        c2.nChannels = c.nChannels;
        return c2;
    }
};
