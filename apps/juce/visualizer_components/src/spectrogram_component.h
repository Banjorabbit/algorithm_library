#pragma once
#include "algorithm_library/spectrogram.h"
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>
class SpectrogramComponent : public juce::Component, juce::Timer
{
  public:
    SpectrogramComponent(float sampleRate)
        : bufferSize(getBufferSize(sampleRate)),
          spectrogram({.bufferSize = bufferSize, .nBands = bufferSize + 1, .algorithmType = SpectrogramConfiguration::Coefficients::HANN}),
          spectrogramImage(juce::Image::RGB, nSpectrogramFrames, bufferSize + 1, true)
    {
        circularBuffer = Eigen::ArrayXf::Zero(getcircularBufferSize(static_cast<int>(.01 * sampleRate), sampleRate));
        writeBufferIndex.store(0);
        readBufferIndex.store(0);
        bufferIn = Eigen::ArrayXf::Zero(bufferSize);
        spectrogramOut = Eigen::ArrayXf::Zero(bufferSize + 1);

        startTimerHz(60);
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
            bufferIn = Eigen::ArrayXf::Zero(bufferSize);
            spectrogramOut = Eigen::ArrayXf::Zero(bufferSize + 1);
        }
        int circularBufferSize = getcircularBufferSize(expectedBufferSize, sampleRate);
        if (circularBufferSize != circularBuffer.size())
        {
            circularBuffer = Eigen::ArrayXf::Zero(circularBufferSize);
            writeBufferIndex.store(0);
            readBufferIndex.store(0);
        }
    }

    // push data into circular buffer. This method is likely to be called from the main audio thread and should not do any heavy calculations or GUI work
    void pushSamples(I::Real buffer)
    {
        int index = writeBufferIndex.load();
        int size = static_cast<int>(buffer.size());
        const int sizeCircular = static_cast<int>(circularBuffer.size());
        if (size > sizeCircular) // fallback if given more samples than we can handle
        {
            index = 0;
            size = sizeCircular;
        }

        const int size1 = std::min(sizeCircular - index, size);
        const int size2 = size - size1;
        circularBuffer.segment(index, size1) = buffer.head(size1);
        circularBuffer.head(size2) = buffer.tail(size2);

        int indexNew = index + size;
        if (indexNew >= sizeCircular) { indexNew -= sizeCircular; }
        writeBufferIndex.store(indexNew);
    }

    void reset()
    {
        circularBuffer.setZero();
        writeBufferIndex.store(0);
        readBufferIndex.store(0);
        spectrogram.reset();
    }

    // read from circular buffer and calculate spectrogram. This method is called from message thread and is not time critical
    void timerCallback() override
    {
        int startIndex = readBufferIndex.load();
        int endIndex = writeBufferIndex.load();
        const int sizeCircularBuffer = static_cast<int>(circularBuffer.size());
        int length = endIndex - startIndex;
        if (length < 0) { length += sizeCircularBuffer; }

        const int nFrames = length / bufferSize;
        for (auto i = 0; i < nFrames; i++)
        {
            endIndex = startIndex + bufferSize;
            const int size1 = bufferSize - std::max(0, endIndex - sizeCircularBuffer);
            const int size2 = bufferSize - size1;
            bufferIn.head(size1) = circularBuffer.segment(startIndex, size1);
            bufferIn.tail(size2) = circularBuffer.head(size2);

            spectrogram.process(bufferIn, spectrogramOut);
            startIndex = endIndex;
            if (startIndex >= circularBuffer.size()) { startIndex -= sizeCircularBuffer; }

            for (auto y = 1; y < bufferSize+1; ++y)
            {
                auto level = juce::jmap(10*std::log10f(std::max(spectrogramOut(y)+1e-20f,-80.f)), -80.f, 0.f, 0.0f, 1.0f);
                spectrogramImage.setPixelAt(framePlot, y, juce::Colour::fromHSV (level, 1.0f, level, 1.0f));
            }
            framePlot++;
            if (framePlot >= nSpectrogramFrames) { framePlot = 0; }
        }
        readBufferIndex.store(startIndex);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        g.setOpacity (1.0f);
        g.drawImage (spectrogramImage, getLocalBounds().toFloat());
    }
  private:
    // bufferSize is around 10ms and half the number of samples in the FFT with 50% overlap
    int getBufferSize(float sampleRate) const { return SpectrogramConfiguration::getValidFFTSize(static_cast<int>(2 * sampleRate * 0.01f)) / 2; }

    // circular buffer size is max of 100ms and 8x the expected buffer size
    int getcircularBufferSize(int expectedBufferSize, float sampleRate) const { return std::max(static_cast<int>(sampleRate * 0.1f), 8 * expectedBufferSize); }

    int bufferSize;
    Eigen::ArrayXf circularBuffer;
    std::atomic<int> writeBufferIndex;
    std::atomic<int> readBufferIndex;
    Spectrogram spectrogram;
    Eigen::ArrayXf bufferIn;
    Eigen::ArrayXf spectrogramOut;
    juce::Image spectrogramImage;
    constexpr static int nSpectrogramFrames = 3000; // number of time frames in spectrogram image
    int framePlot = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramComponent)
};
