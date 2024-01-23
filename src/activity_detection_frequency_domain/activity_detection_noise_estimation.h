#include "algorithm_library/activity_detection_frequency_domain.h"
#include "noise_estimation/noise_estimation_activity_detection.h"
#include "framework/framework.h"

class ActivityDetectionNoiseEstimation : public AlgorithmImplementation<ActivityDetectionFrequencyDomainConfiguration, ActivityDetectionNoiseEstimation>
{
public:
    ActivityDetectionNoiseEstimation(const Coefficients& c = Coefficients()) :
        AlgorithmImplementation<ActivityDetectionFrequencyDomainConfiguration, ActivityDetectionNoiseEstimation>{c},
        noiseActivityDetector(convertToNoiseActivityDetectorCoefficients(c))
    { }

    NoiseEstimationActivityDetection noiseActivityDetector;
    DEFINE_MEMBER_ALGORITHMS(noiseActivityDetector)

    void processOn(Input noisyPower, Output activityFlag)
	{
        Eigen::ArrayXXf noisePower(C.nBands, C.nChannels);
        Eigen::ArrayXXf activity(C.nBands, C.nChannels);
        noiseActivityDetector.process(noisyPower, {noisePower, activity});

        activityFlag = (noisyPower.colwise().mean().transpose() > (noisePower.colwise().mean().transpose() * 1.5)).any(); // mean power of each channel is compared. ActivityFlag is true if any channel is above threshold
        activityFlag |= static_cast<float>((activity > 0.5f * P.activityThreshold).rowwise().any().count()) > (0.15f * P.activityThreshold * C.nBands); // (1) true if activity is above threshold in any channel, (2) then count over bands and compare with threshold
	}

private:
    static NoiseEstimationActivityDetection::Coefficients convertToNoiseActivityDetectorCoefficients(const Coefficients& c)
    {
        NoiseEstimationActivityDetection::Coefficients cNoise;
        cNoise.filterbankRate = c.filterbankRate;
        cNoise.nBands = c.nBands;
        cNoise.nChannels = c.nChannels;
        return cNoise;
    }
};