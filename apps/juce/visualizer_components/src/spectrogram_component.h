#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "algorithm_library/spectrogram.h"

class SpectrogramComponent : public juce::Component
{
public:
    SpectrogramComponent(int bufferSize) : spectrogram({.bufferSize = bufferSize, .nBands = bufferSize-1, .algorithmType = SpectrogramConfiguration::Coefficients::HANN})
    { }

private:
    Spectrogram spectrogram;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrogramComponent)
};