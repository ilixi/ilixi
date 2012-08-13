/*
 Copyright 2010-2012 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>.

 This file is part of ilixi.

 ilixi is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ilixi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with ilixi.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ILIXI_STYLISTBASE_H_
#define ILIXI_STYLISTBASE_H_

#include <types/Rectangle.h>
#include <types/Enums.h>
#include <types/Image.h>
#include <types/Font.h>
#include <graphics/Style.h>

namespace ilixi
{

class Palette;
class TweenAnimation;
class Painter;
class Widget;
// Forward declarations for the rest of standard UI elements.
class Application;
class CheckBox;
class ComboBox;
class Dialog;
class Frame;
class GroupBox;
class Icon;
class Label;
class LineInput;
class ProgressBar;
class PushButton;
class RadioButton;
class Slider;
class TabPanel;
class TabPanelButton;
class ToolButton;

class StylistBase
{
    friend class Application;
    friend class Image;

public:
    enum StyledAnimation
    {
        ExposeIn, ExposeOut, FocusIn, FocusOut
    };

    /*!
     * Constructor.
     */
    StylistBase();

    /*!
     * Destructor.
     */
    virtual
    ~StylistBase();

    /*!
     * Sets the palette using a file.
     * Returns true if successful.
     *
     * @param palette path to a valid palette file.
     */
    virtual bool
    setPaletteFromFile(const char* palette);

    /*!
     * Sets the images/hints/fonts using a file.
     * Returns true if successful.
     *
     * @param style path to a valid style file.
     */
    virtual bool
    setStyleFromFile(const char* style);

    /*!
     * Returns the default size for given type.
     */
    virtual Size
    defaultSize(StyleHint::Size size) const;

    /*!
     * Returns the default parameter value for given type.
     */
    int
    defaultParameter(StyleHint::Parameter parameter) const;

    /*!
     * Returns the default font for given type.
     */
    Font*
    defaultFont(StyleHint::Font font = StyleHint::DefaultFont) const;

    /*!
     * Returns the default icon for given type.
     */
    Image*
    defaultIcon(StyleHint::DefaultIcon icon) const;

    /*!
     * Animated drawing for given widget.
     * @param type
     * @param target
     */
    void
    animate(StyledAnimation type, Widget* target);

    /*!
     * Draws application frame.
     */
    virtual void
    drawAppFrame(Painter* painter, Application* app) =0;

    /*!
     * Draws a dialog's frame and its title.
     */
    virtual void
    drawDialog(Painter* painter, Dialog* dialog)=0;

    /*!
     * Draws a frame.
     */
    virtual void
    drawFrame(Painter* painter, Frame* frame)=0;

    /*!
     * Draws a label.
     */
    virtual void
    drawLabel(Painter* painter, Label* label)=0;

    /*!
     * Draws an icon.
     */
    virtual void
    drawIcon(Painter* painter, Icon* icon)=0;

    /*!
     * Draws a checkbox.
     */
    virtual void
    drawCheckBox(Painter* painter, CheckBox* checkbox)=0;

    /*!
     * Draws a line input.
     */
    virtual void
    drawLineInput(Painter* painter, LineInput* input)=0;

    /*!
     * Draws a groupbox.
     */
    virtual void
    drawGroupBox(Painter* painter, GroupBox* box)=0;

    /*!
     * Draws a pushbutton.
     */
    virtual void
    drawPushButton(Painter* painter, PushButton* button)=0;

    /*!
     * Draws a radiobutton.
     */
    virtual void
    drawRadioButton(Painter* painter, RadioButton* button)=0;

    /*!
     * Draws a progressbar.
     */
    virtual void
    drawProgressBar(Painter* painter, ProgressBar* bar) =0;

    /*!
     * Draws a slider.
     */
    virtual void
    drawSlider(Painter* painter, Slider* slider)=0;

    /*!
     * Draws a tab panel button.
     */
    virtual void
    drawTabPanelButton(Painter* painter, TabPanelButton* button)=0;

    /*!
     * Draws a tab panel.
     */
    virtual void
    drawTabPanel(Painter* painter, TabPanel* panel)=0;

    /*!
     * Draws a toolbutton.
     */
    virtual void
    drawToolButton(Painter* painter, ToolButton* button)=0;

    /*!
     * Draws a combobox.
     */
    virtual void
    drawComboBox(Painter* painter, ComboBox* combo)=0;

    /*!
     * Draws a frame using given coordinates.
     */
    virtual void
    drawFrame(Painter* painter, int x, int y, int w, int h, Corners corners =
                      AllCorners)=0;

protected:
    //! This property stores the palette.
    Palette* _palette;
    //! This property stores the style items.
    Style* _style;
    //! This property specifies the width of borders around widgets.
    int _borderWidth;

    struct AnimatedElement
    {
        TweenAnimation* in;
        TweenAnimation* out;
        Widget* targetIn;
        Widget* targetOut;
    };

    AnimatedElement _focus;

private:
    //! "Image not found" image.
    static Image* _noImage;

    virtual void
    initAnimations();

    void
    runAnimation(StyledAnimation type);
};

} /* namespace ilixi */
#endif /* ILIXI_STYLISTBASE_H_ */