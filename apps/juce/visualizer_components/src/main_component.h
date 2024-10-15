#pragma once
#include "spectrogram_component.h"
#include <juce_gui_basics/juce_gui_basics.h>

class MainComponent final : public juce::AudioAppComponent
{
  public:
    //==============================================================================
    MainComponent() : spectrogram(48000.f), audioSetupComp(deviceManager, 0, 2, 0, 2, false, false, false, false)
    {

        addAndMakeVisible(spectrogram);
        addAndMakeVisible(audioSetupComp);
        
        setSize(750, 500);

        setAudioChannels(1, 2);
    }

    ~MainComponent() override { shutdownAudio(); }

    //==============================================================================
    void paint(juce::Graphics &g) override
    {
        //g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        //g.setFont(juce::FontOptions(16.0f));
        //g.setColour(juce::Colours::white);
        //g.drawText("Hello World!", getLocalBounds(), juce::Justification::centred, true);
    }

    void resized() override 
    { 
        auto rect = getLocalBounds();
        spectrogram.setBounds(0, 0, getWidth(), getHeight()); 
        audioSetupComp.setBounds (rect.removeFromLeft (proportionOfWidth (0.6f)));
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override { spectrogram.prepareToPlay(samplesPerBlockExpected, static_cast<float>(sampleRate)); }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override
    {
        Eigen::Map<const Eigen::ArrayXf> array(bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample), bufferToFill.numSamples);
        spectrogram.pushSamples(array);
    }

    void releaseResources() override {}

  private:
    SpectrogramComponent spectrogram;
    juce::AudioDeviceSelectorComponent audioSetupComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
