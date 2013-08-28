/*
 Copyright 2010-2013 Tarik Sekmen.

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

#include <graphics/FontPack.h>
#include <graphics/IconPack.h>
#include <graphics/Style.h>
#include <graphics/Palette.h>

namespace ilixi
{

class TweenAnimation;
class Painter;
class Widget;
// Forward declarations for the rest of standard UI elements.
class AppWindow;
class CheckBox;
class ComboBox;
class Dialog;
class DirectionalButton;
class Frame;
class GridView;
class GroupBox;
class Icon;
class Label;
class LineInput;
class LineSeperator;
class ListBox;
class ProgressBar;
class PushButton;
class RadioButton;
class ScrollArea;
class ScrollBar;
class Slider;
class SpinBox;
class TabPanel;
class TabPanelButton;
class ToolBar;
class ToolBarButton;
class ToolButton;

class StylistBase
{
    friend class Application;
    friend class Image;

public:

    //! This enum specifies drawing animation type.
    enum StyledAnimation
    {
        ExposeIn,
        ExposeOut,
        FocusIn,
        FocusOut
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
     * Returns the default size for given type.
     */
    virtual Size
    defaultSize(StyleHint::SizeHint size) const;

    /*!
     * Returns the default parameter value for given type.
     */
    int
    defaultParameter(StyleHint::Parameter parameter) const;

    /*!
     * Returns the default font for given type.
     */
    Font*
    defaultFont(StyleHint::FontHint font = StyleHint::DefaultFont) const;

    /*!
     * Returns the default icon for given type.
     */
    Image*
    defaultIcon(StyleHint::PackedIcon icon) const;

    Palette*
    palette() const;

    /*!
     * Animated drawing for given widget.
     *
     * @param type
     * @param target
     */
    void
    animate(StyledAnimation type, Widget* target);

    /*!
     * Draws application frame.
     */
    virtual void
    drawAppFrame(Painter* painter, AppWindow* app) =0;

    /*!
     * Draws a dialog's frame and its title.
     */
    virtual void
    drawDialog(Painter* painter, Dialog* dialog)=0;

    /*!
     * Draws a directional button.
     */
    virtual void
    drawDirectionalButton(Painter* painter, DirectionalButton* button)=0;

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
    drawLineInput(Painter* painter, LineInput* input, bool cursor = false)=0;

    /*!
     * Draws a line seperator.
     */
    virtual void
    drawLineSeperator(Painter* painter, LineSeperator* seperator)=0;

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
    drawProgressBar(Painter* painter, ProgressBar* bar)=0;

    /*!
     * Draws a scrollarea frame.
     */
    virtual void
    drawScrollArea(Painter* painter, ScrollArea* area, const Rectangle& content)=0;

    /*!
     * Draws a scrollbar inside given rectangle and orientation.
     */
    virtual void
    drawScrollBar(Painter* painter, int x, int y, int w, int h, Orientation orientation)=0;

    /*!
     * Draws a scroll bar with indicator.
     */
    virtual void
    drawScrollBar(Painter* painter, ScrollBar* bar)=0;

    /*!
     * Draws a slider.
     */
    virtual void
    drawSlider(Painter* painter, Slider* slider)=0;

    /*!
     * Draws a spinbox.
     */
    virtual void
    drawSpinBox(Painter* painter, SpinBox* spinBox, const Size& size)=0;

    /*!
     * Draws a tab panel button.
     */
    virtual void
    drawTabPanelButton(Painter* painter, TabPanelButton* button, bool first)=0;

    /*!
     * Draws a tab panel.
     */
    virtual void
    drawTabPanel(Painter* painter, TabPanel* panel, int y, int index)=0;

    /*!
     * Draws a toolbar background only.
     */
    virtual void
    drawToolBar(Painter* painter, ToolBar* toolbar)=0;

    /*!
     * Draws a toolbar button.
     */
    virtual void
    drawToolBarButton(Painter* painter, ToolBarButton* button)=0;

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
    drawFrame(Painter* painter, int x, int y, int w, int h)=0;

    /*!
     * Draws a tool button frame using given coordinates.
     */
    virtual void
    drawToolButtonFrame(Painter* painter, int x, int y, int w, int h, WidgetState state)=0;

    virtual void
    drawHeader(Painter* painter, int x, int y, int w, int h)=0;

protected:
    //! This property stores the fonts pack.
    FontPack* _fonts;
    //! This property stores the icons pack.
    IconPack* _icons;
    //! This property stores the palette.
    Palette* _palette;
    //! This property stores the style items.
    Style* _style;

    struct AnimatedElement
    {
        TweenAnimation* in;
        TweenAnimation* out;
        Widget* targetIn;
        Widget* targetOut;
    };

    AnimatedElement _focus;

    /*!
     * Sets the fonts pack using a file.
     * Returns true if successful.
     *
     * @param fontPack path to a valid fonts pack file.
     */
    virtual bool
    setFontPack(const char* fontPack);

    /*!
     * Sets the icons pack using a file.
     * Returns true if successful.
     *
     * @param iconPack path to a valid icons pack file.
     */
    virtual bool
    setIconPack(const char* iconPack);

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
