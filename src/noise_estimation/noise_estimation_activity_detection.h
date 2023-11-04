#pragma once
#include "framework/framework.h"

struct NoiseEstimationConfiguration : public Configuration<I::Real2D, O::Real2D>
{
    struct Coefficients
    {
        float filterbankRate = 125.f;
        int nBands = 257;
        int nChannels = 2;
        DEFINE_TUNABLE_COEFFICIENTS(filterbankRate, nChannels, nBands)
    };

    struct Parameters
    {
        float smoothingTConstant = .72f;
        DEFINE_TUNABLE_PARAMETERS(smoothingTConstant)
    };

    static auto validInput(Input input, const Coefficients& c) 
    { 
        return (input.rows() == c.nBands) && (input.cols() == c.nChannels);
    }
    
    static auto initOutput(Input input, const Coefficients& c) { return Eigen::ArrayXXf(c.nBands, c.nChannels); }

    template<typename Talgo>
    struct Test
    {
        Talgo algo;
        Eigen::ArrayXXf input;
        Eigen::ArrayXXf output;

        Test() : Test(Coefficients()) {}
        Test(const Coefficients& c) : algo(c)
        {
            input = Eigen::ArrayXXf(c.nBands, c.nChannels);
            input.setRandom();
            output = initOutput(input, c);
        }

        void processAlgorithm() { algo.process(input, output); }
        bool isTestOutputFinite() const { return output.allFinite(); }
    };
};

// noise estimation based on an activity detector. The more activity in the power spectra, the more the output is smoothed.
//
// author: Kristian Timm Andersen
class NoiseEstimationActivityDetection : public IAlgorithm<NoiseEstimationConfiguration, NoiseEstimationActivityDetection>
{
public:
    NoiseEstimationActivityDetection(Coefficients c =  Coefficients()) :
        IAlgorithm<NoiseEstimationConfiguration, NoiseEstimationActivityDetection>{ c }
    {
        activityMean.resize(c.nBands, c.nChannels);
        powerNoise.resize(c.nBands, c.nChannels);
        activityMeanLambda = 1.f - expf(-1.f / (c.filterbankRate * .152f));
        reset();
        onParametersChanged();
     }


    inline void processOn(Input input, Output output)
    {
        // activity detection
        Eigen::ArrayXXf activity = (0.9693465699682844f * input / (powerNoise + 1e-20f) - 3.485010713180571f).cwiseMin(25.f).exp();
        activity /= (1.f + activity);

        // smooth activity
        activityMean += activityMeanLambda * (activity - activityMean);
        activity = (activityMean > 0.99f).select(activity.cwiseMin(.99f), activity);

        // update noise
        powerNoise += smoothingLambda * (1.f - activity) * (input - powerNoise);
        output = powerNoise;
    }

    void onParametersChanged()
    {
        smoothingLambda = 1.f - expf(-1.f / (C.filterbankRate * P.smoothingTConstant));
    } 

private:

    void resetMembers() final
    {
        activityMean.setZero();
        powerNoise.setZero();
    }

    size_t getMembersDynamicSize() const final
    {
        size_t size = activityMean.getDynamicMemorySize();
        size += powerNoise.getDynamicMemorySize();
        return size;
    }

    Eigen::ArrayXXf activityMean;
    Eigen::ArrayXXf powerNoise;
    float activityMeanLambda, smoothingLambda;
};
