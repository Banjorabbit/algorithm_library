#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class MainComponent final : public juce::Component
{
public:
    //==============================================================================
    MainComponent() 
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

private:
    //AudioFileReaderComponent<ConvolutionDSP> fileReaderComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
