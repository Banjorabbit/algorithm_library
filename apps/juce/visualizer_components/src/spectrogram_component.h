#pragma once
#include "algorithm_library/spectrogram.h"
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>

class SpectrogramComponent : public juce::Component
{
  public:
    SpectrogramComponent(float sampleRate)
        : bufferSize(getBufferSize(sampleRate)),
          spectrogram({.bufferSize = bufferSize, .nBands = bufferSize + 1, .algorithmType = SpectrogramConfiguration::Coefficients::HANN})
    {
        circularBuffer = Eigen::ArrayXf::Zero(getcircularBufferSize(static_cast<int>(.01 * sampleRate), sampleRate));
        circularBufferIndex.store(0);
    }

    void prepareToPlay(int expectedBufferSize, float sampleRate)
    {
        int bufferSizeNew = getBufferSize(sampleRate);
        if (bufferSizeNew != bufferSize)
        {
            auto c = spectrogram.getCoefficients();
            c.bufferSize = bufferSize;
            c.nBands = bufferSize + 1;
            spectrogram.setCoefficients(c);
            bufferSize = bufferSizeNew;
        }
        int circularBufferSize = getcircularBufferSize(expectedBufferSize, sampleRate);
        if (circularBufferSize != circularBuffer.size())
        {
            circularBuffer = Eigen::ArrayXf::Zero(circularBufferSize);
            circularBufferIndex.store(0);
        }
    }

    // push buffer into circular buffer. This method is likely to be called from the main audio thread and should not do any heavy calculations or GUI work
    void pushSamples(I::Real buffer)
    {
        int index = circularBufferIndex.load();
        int size = static_cast<int>(buffer.size());
        int sizeCircular = static_cast<int>(circularBuffer.size());
        if (size > sizeCircular) // fallback if given more samples than we can handle
        {
            index = 0;
            size = sizeCircular;
        }

        int size1 = std::min(sizeCircular - index, size);
        int size2 = size - size1;
        circularBuffer.segment(index, size1) = buffer.head(size1);
        circularBuffer.head(size2) = buffer.tail(size2);

        int indexNew = index + size;
        if (indexNew >= sizeCircular) { circularBufferIndex.store(indexNew - sizeCircular); }
        else { circularBufferIndex.store(indexNew); }
    }

    void reset()
    {
        circularBuffer.setZero();
        circularBufferIndex.store(0);
        spectrogram.reset();
    }

  private:
    // bufferSize is around 10ms and half the number of samples in the FFT with 50% overlap
    int getBufferSize(float sampleRate) const { return SpectrogramConfiguration::getValidFFTSize(static_cast<int>(2 * sampleRate * 0.01f)) / 2; }

    // circular buffer size is max of 100ms and 8x the expected buffer size
    int getcircularBufferSize(int expectedBufferSize, float sampleRate) const { return std::max(static_cast<int>(sampleRate * 0.1f), 8 * expectedBufferSize); }

    int bufferSize;
    Eigen::ArrayXf circularBuffer;
    std::atomic<int> circularBufferIndex;
    Spectrogram spectrogram;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramComponent)
};