#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "spectrogram_component.h"

class MainComponent final : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent() :spectrogram(48000.f)
    {
        setSize (750, 500);
        //addAndMakeVisible (fileReaderComponent);
    }

    //==============================================================================
    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

        g.setFont (juce::FontOptions (16.0f));
        g.setColour (juce::Colours::white);
        g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
    }

    void resized() override {}

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override 
    {
        spectrogram.prepareToPlay(samplesPerBlockExpected, static_cast<float>(sampleRate));
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override 
    {
        Eigen::Map<const Eigen::ArrayXf> array(bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample), bufferToFill.numSamples);
        spectrogram.pushSamples(array);
    }

    void releaseResources() override { }

private:
    //AudioFileReaderComponent<ConvolutionDSP> fileReaderComponent;
    SpectrogramComponent spectrogram;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
