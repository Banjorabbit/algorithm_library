#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
//#include <juce_gui_extra/juce_gui_extra.h>
#include "JuceHeader.h"
#include "DSP_Common.h"

struct ConvolutionDSP
{
    void prepare (const dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        convolution.prepare (spec);
        updateParameters();
    }

    void process (dsp::ProcessContextReplacing<float> context)
    {
        context.isBypassed = bypass;

        // Load a new IR if there's one available. Note that this doesn't lock or allocate!
        bufferTransfer.get ([this] (BufferWithSampleRate& buf)
        {
            convolution.loadImpulseResponse (std::move (buf.buffer),
                                             buf.sampleRate,
                                             dsp::Convolution::Stereo::yes,
                                             dsp::Convolution::Trim::yes,
                                             dsp::Convolution::Normalise::yes);
        });

        convolution.process (context);
    }

    void reset()
    {
        convolution.reset();
    }

    void updateParameters()
    {
        auto* cabinetTypeParameter = dynamic_cast<ChoiceParameter*> (parameters[0]);

        if (cabinetTypeParameter == nullptr)
        {
            jassertfalse;
            return;
        }

        if (cabinetTypeParameter->getCurrentSelectedID() == 1)
        {
            bypass = true;
        }
        else
        {
            bypass = false;

            auto selectedType = cabinetTypeParameter->getCurrentSelectedID();
            auto assetName = (selectedType == 2 ? "guitar_amp.wav" : "cassette_recorder.wav");

            auto assetInputStream = createAssetInputStream (assetName);

            if (assetInputStream == nullptr)
            {
                jassertfalse;
                return;
            }

            AudioFormatManager manager;
            manager.registerBasicFormats();
            std::unique_ptr<AudioFormatReader> reader { manager.createReaderFor (std::move (assetInputStream)) };

            if (reader == nullptr)
            {
                jassertfalse;
                return;
            }

            AudioBuffer<float> buffer (static_cast<int> (reader->numChannels),
                                       static_cast<int> (reader->lengthInSamples));
            reader->read (buffer.getArrayOfWritePointers(), buffer.getNumChannels(), 0, buffer.getNumSamples());

            bufferTransfer.set (BufferWithSampleRate { std::move (buffer), reader->sampleRate });
        }
    }

    //==============================================================================
    struct BufferWithSampleRate
    {
        BufferWithSampleRate() = default;

        BufferWithSampleRate (AudioBuffer<float>&& bufferIn, double sampleRateIn)
            : buffer (std::move (bufferIn)), sampleRate (sampleRateIn) {}

        AudioBuffer<float> buffer;
        double sampleRate = 0.0;
    };

    class BufferTransfer
    {
    public:
        void set (BufferWithSampleRate&& p)
        {
            const SpinLock::ScopedLockType lock (mutex);
            buffer = std::move (p);
            newBuffer = true;
        }

        // Call `fn` passing the new buffer, if there's one available
        template <typename Fn>
        void get (Fn&& fn)
        {
            const SpinLock::ScopedTryLockType lock (mutex);

            if (lock.isLocked() && newBuffer)
            {
                fn (buffer);
                newBuffer = false;
            }
        }

    private:
        BufferWithSampleRate buffer;
        bool newBuffer = false;
        SpinLock mutex;
    };

    double sampleRate = 0.0;
    bool bypass = false;

    MemoryBlock currentCabinetData;
    dsp::Convolution convolution;

    BufferTransfer bufferTransfer;

    ChoiceParameter cabinetParam { { "Bypass", "Guitar amplifier 8''", "Cassette recorder" }, 1, "Cabinet Type" };

    std::vector<DSPParameterBase*> parameters { &cabinetParam };
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent final : public juce::Component
{
public:
    //==============================================================================
    MainComponent();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...

    AudioFileReaderComponent<ConvolutionDSP> fileReaderComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
