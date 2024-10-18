#pragma once
#include "audio_player_component.h"
#include "audio_dsp_transport_source.h"

class SpectrogramSource : public juce::AudioSource
{
  public:
    virtual void stopPlot() = 0;
    virtual void startPlot() = 0;
};

class AudioStreamComponent final : public juce::Component, public AudioPlayerEnclosing, private juce::TimeSliceThread, private juce::Value::Listener, private juce::ChangeListener
{
public:
    AudioStreamComponent(SpectrogramSource* dspSource = nullptr)
        : TimeSliceThread("Audio File Reader Thread"),
          audioPlayer(audioDeviceManager, formatManager, *this), sourceExternal(dspSource)
    {
        loopState.addListener(this);

        formatManager.registerBasicFormats();
        audioDeviceManager.addAudioCallback(&audioSourcePlayer);

        audioDeviceManager.initialiseWithDefaultDevices(0, 2);

        startThread();

        setOpaque(true);

        addAndMakeVisible(audioPlayer);

        setSize(800, 250);
    }

    ~AudioStreamComponent() override
    {
        signalThreadShouldExit();
        stop();
        audioDeviceManager.removeAudioCallback(&audioSourcePlayer);
        waitForThreadToExit(10000);
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
        g.fillRect(getLocalBounds());
    }

    void resized() override
    {
        audioPlayer.setBounds(getLocalBounds());
    }

    bool loadURL(const juce::URL& fileToPlay)
    {
        stop();

        audioSourcePlayer.setSource(nullptr);
        audioPlayer.setTransportSource(nullptr);
        transportSource.reset();
        readerSource.reset();

        auto source = makeInputSource(fileToPlay);

        if(source == nullptr)
            return false;

        auto stream = rawToUniquePtr(source->createInputStream());

        if(stream == nullptr)
            return false;

        reader = rawToUniquePtr(formatManager.createReaderFor(std::move(stream)));

        if(reader == nullptr)
            return false;

        readerSource.reset(new juce::AudioFormatReaderSource(reader.get(), false));
        readerSource->setLooping(loopState.getValue());

        init();
        resized();

        return true;
    }

     void togglePlay()
    {
        if(playState.getValue())
            stop();
        else
            play();
    }

    void stop()
    {
        playState = false;

        if(transportSource.get() != nullptr)
        {
            transportSource->stop();
            transportSource->setPosition(0);

            if(sourceExternal != nullptr) { sourceExternal->stopPlot(); }
        }
    }

    void init()
    {
        if(transportSource.get() == nullptr)
        {
            if (sourceExternal != nullptr) { transportSource.reset(new AudioDSPTransportSource(*sourceExternal)); }
            else { transportSource.reset(new juce::AudioTransportSource()); }

            transportSource->addChangeListener(this);

            if(readerSource != nullptr)
            {
                if(auto* device = audioDeviceManager.getCurrentAudioDevice())
                {
                    transportSource->setSource(readerSource.get(), juce::roundToInt(device->getCurrentSampleRate()), this, reader->sampleRate);
                    audioPlayer.setTransportSource(transportSource.get());
                }
            }
        }

        audioSourcePlayer.setSource(nullptr);
        audioSourcePlayer.setSource(transportSource.get());
    }

    void play()
    {
        if (readerSource == nullptr)
            return;

        if (transportSource->getCurrentPosition() >= transportSource->getLengthInSeconds() || transportSource->getCurrentPosition() < 0) { transportSource->setPosition(0); }

        transportSource->start();
        playState = true;

        if (sourceExternal != nullptr) { sourceExternal->startPlot(); }
    }

private:

void valueChanged(juce::Value& v) override
    {
        if (readerSource != nullptr)
            readerSource->setLooping(v.getValue());
    }

    void changeListenerCallback(juce::ChangeBroadcaster*) override
    {
        if (playState.getValue() && ! transportSource->isPlaying())
            stop();
    }

    juce::AudioDeviceManager audioDeviceManager;
    juce::AudioFormatManager formatManager;

    std::unique_ptr<juce::AudioFormatReader> reader;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    std::unique_ptr<juce::AudioTransportSource> transportSource;
    SpectrogramSource* sourceExternal;

    juce::AudioSourcePlayer audioSourcePlayer;

    AudioPlayerComponent audioPlayer;
};