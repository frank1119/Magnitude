/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h" 

using namespace juce;

//==============================================================================
// Some helper stuff nicked from the Juce demos
template <typename Func, typename... Items>
constexpr void forEach(Func&& func, Items&&... items)
{
    (func(std::forward<Items>(items)), ...);
}

template <typename... Components>
void addAllAndMakeVisible(Component& target, Components&... children)
{
    forEach([&](Component& child) { target.addAndMakeVisible(child); }, children...);
}

class AttachedSlider : public Component
{
public:
    AttachedSlider(RangedAudioParameter& paramIn)
        : label("", paramIn.name),
        attachment(paramIn, slider)
    {
        slider.addMouseListener(this, true);

        addAllAndMakeVisible(*this, slider, label);

        slider.setTextValueSuffix(" " + paramIn.label);

        label.attachToComponent(&slider, false);
        label.setJustificationType(Justification::centred);
    }

    void resized() override { slider.setBounds(getLocalBounds().reduced(0, 40)); }

private:
    Slider slider{ Slider::RotaryVerticalDrag, Slider::TextBoxBelow };
    Label label;
    SliderParameterAttachment attachment;
};

struct GetTrackInfo
{

    // Sliders take up as much room as they can
    Grid::TrackInfo operator() (AttachedSlider&)            const { return 1_fr; }
};

template <typename... Components>
static void performLayout(const juce::Rectangle<int>& bounds, Components&... components)
{
    Grid grid;
    using Track = Grid::TrackInfo;

    grid.autoColumns = Track(1_fr);
    grid.autoRows = Track(1_fr);
    grid.columnGap = Grid::Px(10);
    grid.rowGap = Grid::Px(0);
    grid.autoFlow = Grid::AutoFlow::column;

    grid.templateColumns = { GetTrackInfo{} (components)... };
    grid.items = { GridItem(components)... };

    grid.performLayout(bounds);
}
//==============================================================================


/**
*/
class MagnitudeAudioProcessorEditor  : public juce::AudioProcessorEditor
    //, public juce::Slider::Listener
{
public:
    MagnitudeAudioProcessorEditor(MagnitudeAudioProcessor&);
    ~MagnitudeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
   // void MagnitudeAudioProcessorEditor::sliderValueChanged(juce::Slider* sliderMoved) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MagnitudeAudioProcessor& audioProcessor;

    std::unique_ptr <AttachedSlider> sldRefreshRate;
    std::unique_ptr <AttachedSlider> sldSmoothing;

    juce::TooltipWindow tooltipWindow;

    // bool resizing = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagnitudeAudioProcessorEditor)
};
