#pragma once
#include "audio_player_component.h"
#include "spectrogram_component.h"
#include <juce_gui_basics/juce_gui_basics.h>

class MainComponent final : public juce::AudioAppComponent, public AudioPlayerEnclosing, private juce::TimeSliceThread, private juce::Value::Listener, private juce::ChangeListener
{
  public:
    //==============================================================================
    MainComponent()
        : juce::TimeSliceThread("Audio File Reader Thread"), spectrogram(48000.f), audioSetupComp(deviceManager, 0, 2, 0, 2, false, false, false, false),
          audioPlayer(deviceManager, formatManager, *this)
    {

        loopState.addListener(this);

        formatManager.registerBasicFormats();

        init();
        startThread();

        setOpaque(true);

        addAndMakeVisible(spectrogram);
        addAndMakeVisible(audioPlayer);
        //addAndMakeVisible(audioSetupComp);

        setSize(750, 500);

        setAudioChannels(1, 2);
    }

    ~MainComponent() override
    {
        signalThreadShouldExit();
        stop();
        shutdownAudio();
        waitForThreadToExit(10000);
    }

    bool loadURL(const juce::URL &fileToPlay) override
    {
        stop();

        shutdownAudio();
        audioPlayer.setTransportSource(nullptr);
        transportSource.reset();
        readerSource.reset();

        auto source = makeInputSource(fileToPlay);
        if (source == nullptr) { return false; }

        auto stream = rawToUniquePtr(source->createInputStream());
        if (stream == nullptr) { return false; }

        reader = rawToUniquePtr(formatManager.createReaderFor(std::move(stream)));
        if (reader == nullptr) { return false; }

        readerSource.reset(new juce::AudioFormatReaderSource(reader.get(), false));
        readerSource->setLooping(loopState.getValue());

        init();
        resized();

        return true;
    }

    void togglePlay() override
    {
        if (playState.getValue()) { stop(); }
        else { play(); }
    }

    void stop() override
    {
        playState = false;

        if (transportSource.get() != nullptr)
        {
            transportSource->stop();
            transportSource->setPosition(0);
        }
    }

    void init()
    {
        if (transportSource.get() == nullptr)
        {
            transportSource.reset(new juce::AudioTransportSource());
            transportSource->addChangeListener(this);

            if (readerSource != nullptr)
            {
                if (auto *device = deviceManager.getCurrentAudioDevice())
                {
                    transportSource->setSource(readerSource.get(), juce::roundToInt(device->getCurrentSampleRate()), this, reader->sampleRate);

                    audioPlayer.setTransportSource(transportSource.get());
                }
            }
        }

        int nInputs = deviceManager.getAudioDeviceSetup().inputChannels.countNumberOfSetBits();
        int nOutputs = deviceManager.getAudioDeviceSetup().outputChannels.countNumberOfSetBits();
        setAudioChannels(nInputs, nOutputs);
    }

    void play()
    {
        if (readerSource == nullptr) { return; }

        if (transportSource->getCurrentPosition() >= transportSource->getLengthInSeconds() || transportSource->getCurrentPosition() < 0) { transportSource->setPosition(0); }

        transportSource->start();
        playState = true;
    }

    //==============================================================================
    void paint(juce::Graphics &g) override
    {
        // g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

        // g.setFont(juce::FontOptions(16.0f));
        // g.setColour(juce::Colours::white);
        // g.drawText("Hello World!", getLocalBounds(), juce::Justification::centred, true);
    }

    void resized() override
    {
        auto rect = getLocalBounds();
        audioPlayer.setBounds(rect.removeFromTop(120));
        spectrogram.setBounds(rect);
        audioSetupComp.setBounds(rect.removeFromLeft(proportionOfWidth(0.6f)));
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override { spectrogram.prepareToPlay(samplesPerBlockExpected, static_cast<float>(sampleRate)); }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override
    {
        Eigen::Map<const Eigen::ArrayXf> array(bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample), bufferToFill.numSamples);
        spectrogram.pushSamples(array);
    }

    void releaseResources() override {}

  private:

    void valueChanged(juce::Value &v) override
    {
        if (readerSource != nullptr)
            readerSource->setLooping (v.getValue());
    }

    void changeListenerCallback(juce::ChangeBroadcaster*) override
    {
        if (playState.getValue() && ! transportSource->isPlaying()) { stop(); }
    }

    SpectrogramComponent spectrogram;
    juce::AudioDeviceSelectorComponent audioSetupComp;

    juce::AudioFormatManager formatManager;

    std::unique_ptr<juce::AudioFormatReader> reader;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    std::unique_ptr<juce::AudioTransportSource> transportSource;

    AudioPlayerComponent audioPlayer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
