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

#ifndef ILIXI_STYLIST_H_
#define ILIXI_STYLIST_H_

#include <graphics/StylistBase.h>

namespace ilixi
{

//! Draws standard widgets.
/*!
 * This class parses a stylesheet and stores palette, images, fonts and various hints.
 * You should override drawing methods in your implementations.
 */
class Stylist : public StylistBase
{
    friend class Application;
    friend class Image;

public:
    /*!
     * Constructor.
     */
    Stylist();

    /*!
     * Destructor.
     */
    virtual
    ~Stylist();

    /*!
     * Draws application frame.
     */
    virtual void
    drawAppFrame(Painter* painter, Application* app);

    /*!
     * Draws a dialog's frame and its title.
     */
    virtual void
    drawDialog(Painter* painter, Dialog* dialog);

    /*!
     * Draws a frame.
     */
    virtual void
    drawFrame(Painter* painter, Frame* frame);

    /*!
     * Draws a label.
     */
    virtual void
    drawLabel(Painter* painter, Label* label);

    /*!
     * Draws an icon.
     */
    virtual void
    drawIcon(Painter* painter, Icon* icon);

    /*!
     * Draws a checkbox.
     */
    virtual void
    drawCheckBox(Painter* painter, CheckBox* checkbox);

    /*!
     * Draws a line input.
     */
    virtual void
    drawLineInput(Painter* painter, LineInput* input);

    /*!
     * Draws a groupbox.
     */
    virtual void
    drawGroupBox(Painter* painter, GroupBox* box);

    /*!
     * Draws a pushbutton.
     */
    virtual void
    drawPushButton(Painter* painter, PushButton* button);

    /*!
     * Draws a radiobutton.
     */
    virtual void
    drawRadioButton(Painter* painter, RadioButton* button);

    /*!
     * Draws a progressbar.
     */
    virtual void
    drawProgressBar(Painter* painter, ProgressBar* bar);

    /*!
     * Draws a slider.
     */
    virtual void
    drawSlider(Painter* painter, Slider* slider);

    /*!
     * Draws a tab panel button.
     */
    virtual void
    drawTabPanelButton(Painter* painter, TabPanelButton* button);

    /*!
     * Draws a tab panel.
     */
    virtual void
    drawTabPanel(Painter* painter, TabPanel* panel);

    /*!
     * Draws a toolbutton.
     */
    virtual void
    drawToolButton(Painter* painter, ToolButton* button);

    /*!
     * Draws a combobox.
     */
    virtual void
    drawComboBox(Painter* painter, ComboBox* combo);

    virtual void
    drawFrame(Painter* painter, int x, int y, int w, int h, Corners corners =
                      AllCorners);

protected:
    virtual void
    draw9Frame(Painter* painter, int x, int y, int w, int h,
               const Style::r9& rect, Corners corners = AllCorners);

    void
    drawTabFrame(Painter* p, int x, int y, int w, int h, const Style::r9& rect);

};
}

#endif /* ILIXI_STYLIST_H_ */
