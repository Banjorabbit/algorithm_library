#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

inline std::unique_ptr<juce::InputSource> makeInputSource(const juce::URL &url)
{
    if (const auto doc = juce::AndroidDocument::fromDocument(url)) return std::make_unique<juce::AndroidDocumentInputSource>(doc);

#if !JUCE_IOS
    if (url.isLocalFile()) return std::make_unique<juce::FileInputSource>(url.getLocalFile());
#endif

    return std::make_unique<juce::URLInputSource>(url);
}

// Thumbnail of an audio file

class AudioThumbnailComponent final : public juce::Component,
                                      public juce::FileDragAndDropTarget,
                                      public juce::ChangeBroadcaster,
                                      private juce::ChangeListener,
                                      private juce::Timer
{
  public:
    AudioThumbnailComponent(juce::AudioDeviceManager &audioDeviceManagerNew, juce::AudioFormatManager &audioFileManagerNew)
        : audioDeviceManager(audioDeviceManagerNew), thumbnailCache(5), thumbnail(128, audioFileManagerNew, thumbnailCache)
    {
        thumbnail.addChangeListener(this);
    }

    ~AudioThumbnailComponent() override { thumbnail.removeChangeListener(this); }

    void paint(juce::Graphics &g) override
    {
        g.fillAll(juce::Colour(0xff495358));

        g.setColour(juce::Colours::white);

        if (thumbnail.getTotalLength() > 0.0)
        {
            thumbnail.drawChannels(g, getLocalBounds().reduced(2), 0.0, thumbnail.getTotalLength(), 1.0f);

            g.setColour(juce::Colours::black);
            g.fillRect(static_cast<float>(currentPosition * getWidth()), 0.0f, 1.0f, static_cast<float>(getHeight()));
        }
        else { g.drawFittedText("No audio file loaded.\nDrop a file here or click the \"Load File...\" button.", getLocalBounds(), juce::Justification::centred, 2); }
    }

    bool isInterestedInFileDrag(const juce::StringArray &) override { return true; }
    void filesDropped(const juce::StringArray &files, int, int) override { loadURL(juce::URL(juce::File(files[0])), true); }

    void setCurrentURL(const juce::URL &u)
    {
        if (currentURL == u) return;

        loadURL(u);
    }

    juce::URL getCurrentURL() const { return currentURL; }

    void setTransportSource(juce::AudioTransportSource *newSource)
    {
        transportSource = newSource;

        struct ResetCallback final : public juce::CallbackMessage
        {
            ResetCallback(AudioThumbnailComponent &o) : owner(o) {}
            void messageCallback() override { owner.reset(); }

            AudioThumbnailComponent &owner;
        };

        (new ResetCallback(*this))->post();
    }

  private:
    //==============================================================================
    void changeListenerCallback(ChangeBroadcaster *) override { repaint(); }

    void reset()
    {
        currentPosition = 0.0;
        repaint();

        if (transportSource == nullptr) stopTimer();
        else startTimerHz(25);
    }

    void loadURL(const juce::URL &u, bool notify = false)
    {
        if (currentURL == u) return;

        currentURL = u;

        thumbnail.setSource(makeInputSource(u).release());

        if (notify) sendChangeMessage();
    }

    void timerCallback() override
    {
        if (transportSource != nullptr)
        {
            currentPosition = transportSource->getCurrentPosition() / thumbnail.getTotalLength();
            repaint();
        }
    }

    void mouseDrag(const juce::MouseEvent &e) override
    {
        if (transportSource != nullptr)
        {
            const juce::ScopedLock sl(audioDeviceManager.getAudioCallbackLock());

            transportSource->setPosition((juce::jmax(static_cast<double>(e.x), 0.0) / getWidth()) * thumbnail.getTotalLength());
        }
    }

    juce::AudioDeviceManager &audioDeviceManager;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;
    juce::AudioTransportSource *transportSource = nullptr;

    juce::URL currentURL;
    double currentPosition = 0.0;
};