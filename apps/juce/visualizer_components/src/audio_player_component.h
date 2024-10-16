#pragma once
#include "audio_thumbnail_component.h"
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

// This class should be inherited from the class that encloses AudioPlayerComponent and is passed to its constructor
class AudioPlayerEnclosing
{
  public:
    virtual void togglePlay() = 0;
    virtual void stop() = 0;
    virtual bool loadURL(const juce::URL &fileToPlay) = 0;

    juce::Value playState{juce::var(false)};
    juce::Value loopState{juce::var(false)};
};

// AudioPlayerComponent contains:
// play file button
// load file button
// loop file checkmark
// AudioThumbnailComponent showing audio file
class AudioPlayerComponent final : public juce::Component, private juce::ChangeListener, private juce::Value::Listener
{
  public:
    AudioPlayerComponent(juce::AudioDeviceManager &audioDeviceManagerNew, juce::AudioFormatManager &audioFileManagerNew, AudioPlayerEnclosing &audioFileReaderNew)
        : thumbnailComp(audioDeviceManagerNew, audioFileManagerNew), audioFileReader(audioFileReaderNew)
    {
        setOpaque(true);

        addAndMakeVisible(loadButton);
        addAndMakeVisible(playButton);
        addAndMakeVisible(loopButton);

        playButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff79ed7f));
        playButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

        loadButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff797fed));
        loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

        loadButton.onClick = [this] { openFile(); };
        playButton.onClick = [this] { audioFileReader.togglePlay(); };

        addAndMakeVisible(thumbnailComp);
        thumbnailComp.addChangeListener(this);

        audioFileReader.playState.addListener(this);
        loopButton.getToggleStateValue().referTo(audioFileReader.loopState);
    }

    ~AudioPlayerComponent() override { audioFileReader.playState.removeListener(this); }

    void paint(juce::Graphics &g) override
    {
        g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker());
        g.fillRect(getLocalBounds());
    }

    void resized() override
    {
        auto bounds = getLocalBounds();

        auto buttonBounds = bounds.removeFromLeft(juce::jmin(250, bounds.getWidth() / 4));
        auto loopBounds = buttonBounds.removeFromBottom(30);

        loadButton.setBounds(buttonBounds.removeFromTop(buttonBounds.getHeight() / 2));
        playButton.setBounds(buttonBounds);

        loopButton.setSize(0, 25);
        loopButton.changeWidthToFitText();
        loopButton.setCentrePosition(loopBounds.getCentre());

        thumbnailComp.setBounds(bounds);
    }

    void setTransportSource(juce::AudioTransportSource *newSource) { thumbnailComp.setTransportSource(newSource); }

  private:
    void openFile()
    {
        audioFileReader.stop();

        if (fileChooser != nullptr) return;

        if (!juce::RuntimePermissions::isGranted(juce::RuntimePermissions::readExternalStorage))
        {
            SafePointer<AudioPlayerComponent> safeThis(this);
            juce::RuntimePermissions::request(juce::RuntimePermissions::readExternalStorage, [safeThis](bool granted) mutable {
                if (safeThis != nullptr && granted) safeThis->openFile();
            });
            return;
        }

        fileChooser.reset(new juce::FileChooser("Select an audio file...", juce::File(), "*.wav;*.mp3;*.aif"));

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser &fc) mutable {
                if (fc.getURLResults().size() > 0)
                {
                    const auto u = fc.getURLResult();

                    if (!audioFileReader.loadURL(u))
                    {
                        auto options = juce::MessageBoxOptions()
                                           .withIconType(juce::MessageBoxIconType::WarningIcon)
                                           .withTitle("Error loading file")
                                           .withMessage("Unable to load audio file")
                                           .withButton("OK");
                        messageBox = juce::NativeMessageBox::showScopedAsync(options, nullptr);
                    }
                    else { thumbnailComp.setCurrentURL(u); }
                }

                fileChooser = nullptr;
            },
            nullptr);
    }

    void changeListenerCallback(juce::ChangeBroadcaster *) override
    {
        if (audioFileReader.playState.getValue()) audioFileReader.stop();

        audioFileReader.loadURL(thumbnailComp.getCurrentURL());
    }

    void valueChanged(juce::Value &v) override
    {
        playButton.setButtonText(v.getValue() ? "Stop" : "Play");
        playButton.setColour(juce::TextButton::buttonColourId, v.getValue() ? juce::Colour(0xffed797f) : juce::Colour(0xff79ed7f));
    }

    //==============================================================================
    juce::TextButton loadButton{"Load File..."}, playButton{"Play"};
    juce::ToggleButton loopButton{"Loop File"};

    AudioThumbnailComponent thumbnailComp;
    AudioPlayerEnclosing &audioFileReader;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::ScopedMessageBox messageBox;
};