#pragma once
#include "framework/framework.h"
#include "algorithm_library/mel_scale.h"

// Mel Scale
//
// author: Kristian Timm Andersen

class MelScaleSpectrogram : public AlgorithmImplementation<MelScaleConfiguration, MelScaleSpectrogram>
{
public:
    MelScaleSpectrogram(Coefficients c = Coefficients()) :
        AlgorithmImplementation<MelScaleConfiguration, MelScaleSpectrogram>{ c }
    { 
        float highFreqMel = 2595 * std::log10(1 + (c.sampleRate/2)/700); // convert Hz to Mel
        Eigen::ArrayXf mels = Eigen::ArrayXf::LinSpaced(c.nMels + 1, 0, highFreqMel); // linear spaced corner indices in Mel domain
        Eigen::ArrayXf freqs = 700 * (Eigen::ArrayXf::Constant(c.nMels+1, 10).pow(mels/2595) - 1); // convert Mel to Hz
        Eigen::ArrayXi cornerBins = ((c.nBands - 1) * 2 / c.sampleRate * freqs).round().cast<int>(); // corner bins, including 0 and nBands. Use round() instead of floor() to ensure increasing mel bin sizes
        indexStart = cornerBins.head(c.nMels);
        nBandsSum = (cornerBins.segment(1, c.nMels) - indexStart).cwiseMax(1);
        indexEnd = indexStart(c.nMels - 1) + nBandsSum(c.nMels - 1);        
    }

    inline void inverse(I::Real2D xPower, O::Real2D yPower)
    {
        assert(xPower.rows() == C.nMels);

        for (auto channel = 0; channel < xPower.cols(); channel++)
        {
            yPower.block(0, channel, indexStart(0), 1).setConstant(xPower(0, channel)/ nBandsSum(0));
            for (auto i = 0; i < C.nMels; i++)
            {
                yPower.block(indexStart(i), channel, nBandsSum(i), 1).setConstant(xPower(i, channel) / nBandsSum(i));
            }
            yPower.block(indexEnd, channel, C.nBands - indexEnd, 1).setConstant(xPower(C.nMels - 1, channel) / nBandsSum(C.nMels-1));
        }
    }

    Eigen::ArrayXf getCornerFrequencies() const 
    { 
        Eigen::ArrayXf array(C.nMels+1);
        array.head(C.nMels) = indexStart.cast<float>();
        array.tail(1) = static_cast<float>(indexEnd);
        array *= C.sampleRate / ((C.nBands - 1) * 2);
        return array;
    }

private:

    inline void processOn(Input input, Output output)
    {
        for (auto channel = 0; channel < input.cols(); channel++)
        {
            for (auto i = 0; i < C.nMels; i++)
            {
                // sum
                output(i, channel) = input.block(indexStart(i), channel, nBandsSum(i), 1).sum();
            }
        }
    }

    size_t getDynamicSizeVariables() const final
    { 
        size_t size = indexStart.getDynamicMemorySize();
        size += nBandsSum.getDynamicMemorySize();
        return size;
    }

    int indexEnd;
    Eigen::ArrayXi indexStart;
    Eigen::ArrayXi nBandsSum;

    friend AlgorithmImplementation<MelScaleConfiguration, MelScaleSpectrogram>;
};