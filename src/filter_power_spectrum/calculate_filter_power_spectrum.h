#pragma once
#include "algorithm_library/filter_power_spectrum.h"
#include "framework/framework.h"

// calculate power spectrum of a cascade of 2nd order filters.
//
// author: Kristian Timm Andersen

class CalculateFilterPowerSpectrum : public AlgorithmImplementation<FilterPowerSpectrumConfiguration, CalculateFilterPowerSpectrum>
{
  public:
    CalculateFilterPowerSpectrum(const Coefficients &c = Coefficients()) : BaseAlgorithm{c}
    {
        // uniform frequency points from 0 to pi. Use double precision for cos calculation
        Eigen::ArrayXd freqsDouble = Eigen::ArrayXd::LinSpaced(c.nBands, 0, 3.141592653589793);
        freqsCos = freqsDouble.cos().cast<float>();
        freqs2Cos = (2 * freqsDouble).cos().cast<float>();
    }

  private:
    // get power frequency response evaluated uniformly from 0 to pi in nBands points
    inline Eigen::ArrayXf getPowerFrequencyResponse(I::Real c) const
    {
        const float b0 = c(0);
        const float b1 = c(1);
        const float b2 = c(2);
        const float a1 = c(4);
        const float a2 = c(5);
        return (b0 * b0 + b1 * b1 + b2 * b2 + 2 * (b0 * b1 + b1 * b2) * freqsCos + 2 * b0 * b2 * freqs2Cos) /
               (1.f + a1 * a1 + a2 * a2 + 2 * (a1 + a1 * a2) * freqsCos + 2 * a2 * freqs2Cos).max(1e-20f);
    }

    inline void processOn(Input input, Output output)
    {
        // first 2nd order filter
        output = getPowerFrequencyResponse(input.col(0));

        // additional 2nd order filters
        for (auto i = 1; i < input.cols(); i++)
        {
            output *= getPowerFrequencyResponse(input.col(i));
        }
    }

    size_t getDynamicSizeVariables() const final
    {
        size_t size = freqsCos.getDynamicMemorySize();
        size += freqs2Cos.getDynamicMemorySize();
        return size;
    }

    Eigen::ArrayXf freqsCos;
    Eigen::ArrayXf freqs2Cos;

    friend BaseAlgorithm;
};
