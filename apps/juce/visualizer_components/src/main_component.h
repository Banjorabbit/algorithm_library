#pragma once
#include "audio_player_component.h"
#include "spectrogram_component.h"
#include "audio_stream_component.h"
#include <juce_gui_basics/juce_gui_basics.h>



class MainComponent final : public juce::Component, SpectrogramSource
{
  public:
    //==============================================================================
    MainComponent()
        : audioPlayer(this), spectrogram(48000.f) //, audioSetupComp(deviceManager, 0, 2, 0, 2, false, false, false, false),
    {
        addAndMakeVisible(spectrogram);
        addAndMakeVisible(audioPlayer);
        //addAndMakeVisible(audioSetupComp);

        setSize(800, 500);        
    }

    void stopPlot() override { spectrogram.stopPlot(); }
    void startPlot() override { spectrogram.startPlot(); }

    void paint(juce::Graphics &g) override
    { }

    void resized() override
    {
        auto rect = getLocalBounds();
        audioPlayer.setBounds(rect.removeFromTop(120));
        spectrogram.setBounds(rect);
        //audioSetupComp.setBounds(rect.removeFromLeft(proportionOfWidth(0.6f)));
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
    AudioStreamComponent audioPlayer;
    
    //juce::AudioDeviceSelectorComponent audioSetupComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
