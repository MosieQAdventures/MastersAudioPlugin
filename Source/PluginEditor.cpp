/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

class meterThread;

void LookAndFeel::drawRotarySlider(juce::Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
{
    using namespace juce;

    auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
    auto fill = slider.findColour(Slider::rotarySliderFillColourId);

    auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(3);

    auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = jmin(5.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;


    //background arc, dial bg path
    Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true);

    //line
    Point<float> thumbPoint(bounds.getCentreX() + (arcRadius)*std::cos(toAngle - MathConstants<float>::halfPi),
        bounds.getCentreY() + (arcRadius)*std::sin(toAngle - MathConstants<float>::halfPi));

    g.setColour(slider.findColour(Slider::thumbColourId));
    g.drawLine(backgroundArc.getBounds().getCentreX(),
        backgroundArc.getBounds().getCentreY(),
        thumbPoint.getX(),
        thumbPoint.getY(),
        lineW);

    //bg arc cd.
    g.setColour(outline);
    g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));

    //value arc / fill path
    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
            bounds.getCentreY(),
            arcRadius,
            arcRadius,
            0.0f,
            rotaryStartAngle,
            toAngle,
            true);

        g.setColour(fill);
        g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
    }

    //point
    auto thumbWidth = lineW * 1.33f;

    g.setColour(Colour(121u, 27u, 27u));
    g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
}

//============================================================

void RotarySliderWithLabels::paint(juce::Graphics& g)
{
    using namespace juce;

    auto startAng = degreesToRadians(180.f + 90.f);
    auto endAng = degreesToRadians(180.f) + MathConstants<float>::twoPi;

    auto range = getRange();

    auto sliderBounds = getSliderBounds();

    //g.setColour(Colours::red);
    //g.drawRect(getLocalBounds());
    //g.setColour(Colours::white);
    //g.drawRoundedRectangle(sliderBounds.toFloat(), 2.f, 2.f);

    this->setColour(juce::Slider::ColourIds::rotarySliderFillColourId, Colour(165u, 56u, 56u));
    this->setColour(juce::Slider::ColourIds::thumbColourId, Colour(25u, 25u, 25u));
    this->setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, Colour(63u, 63u, 63u));

    getLookAndFeel().drawRotarySlider(g,
        sliderBounds.getX(),
        sliderBounds.getY(),
        sliderBounds.getWidth(),
        sliderBounds.getHeight(),
        jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
        startAng,
        endAng,
        *this);

    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth() * 0.5f;

    Path p;

    auto str = getDisplayString();

    auto c = center.getPointOnCircumference(radius - getTextHeight() * 0.4, MathConstants<float>::pi);

    Rectangle<float> r;
    r.setSize(g.getCurrentFont().getStringWidth(str), getTextBoxHeight());
    r.setCentre(c);
    r.setY(r.getY() + getTextHeight());

    p.addRoundedRectangle(r, 2.f);

    g.setColour(Colours::white);
    g.setFont(getTextHeight());
    g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    //25504
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

    size -= getTextHeight() * 1.5;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), bounds.getCentreY());
    r.setY(2);

    return r;
}


juce::String RotarySliderWithLabels::getDisplayString() const
{
    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param))
        return choiceParam->getCurrentChoiceName();

    juce::String str;
    bool addK = false;

    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
    {
        float val = getValue();
        if (val > 999.0f)
        {
            val /= 1000.f;
            addK = true;
        }

        str = juce::String(val, (addK ? 2 : 0));
    }
    else
    {
        jassertfalse;
    }

    if (suffix.isNotEmpty())
    {
        str << " ";
        if (addK)
            str << "k";

        str << suffix;
    }

    return str;

    //return juce::String(getValue());
}

//==============================================================================
MastersPluginVer2023AudioProcessorEditor::MastersPluginVer2023AudioProcessorEditor (MastersPluginVer2023AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),

    macro1Slider(*audioProcessor.apvts.getParameter("Macro 1"), "%"),
    macro2Slider(*audioProcessor.apvts.getParameter("Macro 2"), "%"),
    macro3Slider(*audioProcessor.apvts.getParameter("Macro 3"), "%"),
    macro4Slider(*audioProcessor.apvts.getParameter("Macro 4"), "%"),
    macro5Slider(*audioProcessor.apvts.getParameter("Macro 5"), "%"),
    macro6Slider(*audioProcessor.apvts.getParameter("Macro 6"), "%"),
    macro7Slider(*audioProcessor.apvts.getParameter("Macro 7"), "%"),
    macro8Slider(*audioProcessor.apvts.getParameter("Macro 8"), "%"),

    macro1SliderAttachment(audioProcessor.apvts, "Macro 1", macro1Slider),
    macro2SliderAttachment(audioProcessor.apvts, "Macro 2", macro2Slider),
    macro3SliderAttachment(audioProcessor.apvts, "Macro 3", macro3Slider),
    macro4SliderAttachment(audioProcessor.apvts, "Macro 4", macro4Slider),
    macro5SliderAttachment(audioProcessor.apvts, "Macro 5", macro5Slider),
    macro6SliderAttachment(audioProcessor.apvts, "Macro 6", macro6Slider),
    macro7SliderAttachment(audioProcessor.apvts, "Macro 7", macro7Slider),
    macro8SliderAttachment(audioProcessor.apvts, "Macro 8", macro8Slider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    for (auto* comp : getComps()) {
        addAndMakeVisible(comp);
    }

    setSize (400, 250);
}

MastersPluginVer2023AudioProcessorEditor::~MastersPluginVer2023AudioProcessorEditor()
{
}

//==============================================================================
void MastersPluginVer2023AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

    // 33u, 33u, 33u
    g.fillAll(juce::Colour(28u, 28u, 28u));
}

void MastersPluginVer2023AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();

    auto top4Area = bounds.removeFromTop(bounds.getHeight() * 0.5);
    auto bottom4Area = bounds;

    auto macro1Area = top4Area.removeFromLeft(top4Area.getWidth() * 0.25);
    auto macro2Area = top4Area.removeFromLeft(top4Area.getWidth() * 0.33);
    auto macro3Area = top4Area.removeFromLeft(top4Area.getWidth() * 0.5);
    auto macro4Area = top4Area;

    macro1Slider.setBounds(macro1Area);
    macro2Slider.setBounds(macro2Area);
    macro3Slider.setBounds(macro3Area);
    macro4Slider.setBounds(macro4Area);

    auto macro5Area = bottom4Area.removeFromLeft(bottom4Area.getWidth() * 0.25);
    auto macro6Area = bottom4Area.removeFromLeft(bottom4Area.getWidth() * 0.33);
    auto macro7Area = bottom4Area.removeFromLeft(bottom4Area.getWidth() * 0.5);
    auto macro8Area = bottom4Area;

    macro5Slider.setBounds(macro5Area);
    macro6Slider.setBounds(macro6Area);
    macro7Slider.setBounds(macro7Area);
    macro8Slider.setBounds(macro8Area);

    


}

//================= CUSTOM ====================

std::vector<juce::Component*> MastersPluginVer2023AudioProcessorEditor::getComps() {
    return {
        &macro1Slider,
        &macro2Slider,
        &macro3Slider,
        &macro4Slider,
        &macro5Slider,
        &macro6Slider,
        &macro7Slider,
        &macro8Slider
    };
}



