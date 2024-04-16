#pragma once
#include "algorithm_library/gain_calculation.h"
#include "framework/framework.h"
#include "utilities/fastonebigheader.h"

// Gain Calculation based on apriori estimation given the aposteriori SNR as input: P_{s+n}/Phat_{n}
//
// author: Kristian Timm Andersen
class GainCalculationApriori : public AlgorithmImplementation<GainCalculationConfiguration, GainCalculationApriori>
{
  public:
    GainCalculationApriori(Coefficients c = Coefficients()) : BaseAlgorithm{c}
    {
        snr.resize(c.nBands, c.nChannels);
        gainSmoothed.resize(c.nBands, c.nChannels);
        resetVariables();
        onParametersChanged();
    }

  private:
    inline void processOn(Input snrAposteriori, Output gain)
    {
        snr = snrAposteriori.max(1.00001f);                                                         // limit aposteriori snr slightly above 1.f
        snr = .98f * snr * gainSmoothed.abs2() + 0.02f * snr - 0.02f;                               // snr = 0.98 * Phat_s / Phat_n + 0.02 * (P_{s+n} / Phat_n - 1)
        snr = snr / (snr + 1.f);                                                                    // Wiener gain = snr_apriori / (snr_apriori + 1)
        snr = snr.unaryExpr([&](float x) { return fasterpow(x, P.exponential); }).max(minimumGain); // gain = max(pow(Wiener gain, exponential), minimumGain)

        // smooth over time
        snr -= gainSmoothed;                                                  // gain difference
        gainSmoothed += (snr > 0.f).select(upLambda * snr, downLambda * snr); // smooth gain in time
        gain = gainSmoothed;
    }

    void onParametersChanged()
    {
        minimumGain = fasterpow2(P.minimumGaindB * 0.166096404744368f); // 10 ^ ( minimumdB / 20) = 2 ^ ( minimumdB * log2(10)/20) ) = 2 ^ ( minimumdB * 0.166096404744368)
        upLambda = 1.f - fasterexp(-1.f / (P.upTConstant * C.filterbankRate));
        downLambda = 1.f - fasterexp(-1.f / (P.downTConstant * C.filterbankRate));
    }

    void resetVariables() final { gainSmoothed.setOnes(); }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = gainSmoothed.getDynamicMemorySize();
        size += snr.getDynamicMemorySize();
        return size;
    }

    Eigen::ArrayXXf gainSmoothed;
    Eigen::ArrayXXf snr;
    float minimumGain;
    float upLambda;
    float downLambda;

    friend BaseAlgorithm;
};
