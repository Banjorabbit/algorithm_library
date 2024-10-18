#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

// Wrapper for AudioTransportSource that allows to process audio data before it is played
class AudioDSPTransportSource : public juce::AudioTransportSource
{   
public:
    AudioDSPTransportSource(juce::AudioSource& input) : processing(input)    {}
    
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        juce::AudioTransportSource::getNextAudioBlock(bufferToFill);
        processing.getNextAudioBlock(bufferToFill);
    }

    void releaseResources() override
    {
        juce::AudioTransportSource::releaseResources();
        processing.releaseResources();
    }

    void prepareToPlay(int samplesPerBlockExpected, double newSampleRate) override
    {
        juce::AudioTransportSource::prepareToPlay(samplesPerBlockExpected, newSampleRate);
        processing.prepareToPlay(samplesPerBlockExpected, newSampleRate);
    }

private:
    juce::AudioSource& processing;
};