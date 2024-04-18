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
    inline void processOn(Input input, Output output)
    {
        // first 2nd order filter
        const float b0 = input(0,0);
        const float b1 = input(1,0);
        const float b2 = input(2,0);
        const float a1 = input(4,0);
        const float a2 = input(5,0);
        output = (b0 * b0 + b1 * b1 + b2 * b2 + 2 * (b0 * b1 + b1 * b2) * freqsCos + 2 * b0 * b2 * freqs2Cos) /
               (1.f + a1 * a1 + a2 * a2 + 2 * (a1 + a1 * a2) * freqsCos + 2 * a2 * freqs2Cos).max(1e-20f);
        
        // additional 2nd order filters (code has to be duplicated to avoid heap allocation when doing output *= ...)
        for (auto i = 1; i < input.cols(); i++)
        {
            const float b0 = input(0,i);
            const float b1 = input(1,i);
            const float b2 = input(2,i);
            const float a1 = input(4,i);
            const float a2 = input(5,i);
            output *= (b0 * b0 + b1 * b1 + b2 * b2 + 2 * (b0 * b1 + b1 * b2) * freqsCos + 2 * b0 * b2 * freqs2Cos) /
               (1.f + a1 * a1 + a2 * a2 + 2 * (a1 + a1 * a2) * freqsCos + 2 * a2 * freqs2Cos).max(1e-20f);
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
