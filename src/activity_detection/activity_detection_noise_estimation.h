#include "algorithm_library/activity_detection.h"
#include "framework/framework.h"
#include "utilities/functions.h"

class ActivityDetectionNoiseEstimation : public AlgorithmImplementation<ActivityDetectionConfiguration, ActivityDetectionNoiseEstimation>
{
public:
    ActivityDetectionNoiseEstimation(Coefficients c =  Coefficients()) :
        AlgorithmImplementation<ActivityDetectionConfiguration, ActivityDetectionNoiseEstimation>{ c }
    {
        activityMean.resize(c.nBands, c.nChannels);
        powerNoise.resize(c.nBands, c.nChannels);
        activityMeanLambda = 1.f - expf(-1.f / (c.filterbankRate * .152f));
        resetVariables();
        onParametersChanged();
     }

    inline void processOn(Input powerNoisy, Output activity)
    {
        // activity detection
        activity = (powerNoisy / (powerNoise + 1e-20f) - 3.5f).cwiseMin(25.f);

        // for-loop has been profiled to be faster in gcc than the commented line below:
        //activity =activity.unaryExpr(std::ref(fasterExp));
        float* ptr = activity.data();
        for (auto i = 0; i < activity.size(); i++, ptr++)
        {
            *ptr = fasterExp(*ptr);
        }
            
        activity /= (1.f + activity);

        // smooth activity
        activityMean += activityMeanLambda * (activity - activityMean);
        activity = (activityMean > 0.99f).select(activity.cwiseMin(.99f), activity);

        // update noise
        powerNoise += smoothingLambda * (1.f - activity) * (powerNoisy - powerNoise);
    }

    void onParametersChanged()
    {
        smoothingLambda = 1.f - expf(-1.f / (C.filterbankRate * P.smoothingTConstant));
    }

    inline Eigen::ArrayXXf getPowerNoise() const { return powerNoise; }

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

class ActivityDetectionFusedNoiseEstimation : public AlgorithmImplementation<ActivityDetectionFusedConfiguration, ActivityDetectionFusedNoiseEstimation>
{
public:
    ActivityDetectionFusedNoiseEstimation(Coefficients c =  Coefficients()) :
        AlgorithmImplementation<ActivityDetectionFusedConfiguration, ActivityDetectionFusedNoiseEstimation>{ c },
        activityDetection(convertToActivityDetectionConfiguration(c))
    { }

    ActivityDetectionNoiseEstimation activityDetection;
    DEFINE_MEMBER_ALGORITHMS(activityDetection)

    inline void processOn(Input powerNoisy, Output activityFlag)
    {
        Eigen::ArrayXXf activity(C.nBands, C.nChannels);
        activityDetection.processOn(powerNoisy, activity);
        
        activityFlag = (powerNoisy.colwise().mean() > (getPowerNoise().colwise().mean() * 1.5 * P.activityThreshold)).any(); // mean power of each channel is compared. ActivityFlag is true if any channel is above threshold
        activityFlag |= static_cast<float>((activity > 0.5f * P.activityThreshold).rowwise().any().count()) > (0.15f * P.activityThreshold * C.nBands); // (1) true if activity is above threshold in any channel, (2) then count over bands and compare with threshold
    }

    inline Eigen::ArrayXXf getPowerNoise() const { return activityDetection.getPowerNoise(); }

private:

    ActivityDetectionNoiseEstimation::Coefficients convertToActivityDetectionConfiguration(const Coefficients& c)
    {
        ActivityDetectionNoiseEstimation::Coefficients c2;
        c2.filterbankRate = c.filterbankRate;
        c2.nBands = c.nBands;
        c2.nChannels = c.nChannels;
        return c2;
    }
};