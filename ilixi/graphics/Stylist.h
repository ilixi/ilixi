/*
 Copyright 2011 Tarik Sekmen.

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

#include "types/Rectangle.h"
#include "types/Enums.h"
#include "types/Image.h"
#include "types/Font.h"
#include "graphics/Palette.h"
#include "graphics/Style.h"
#include <libxml/tree.h>

namespace ilixi
{
  class TweenAnimation;
  class Painter;
  class Widget;
  // Forward declarations for the rest of standard UI elements.
  class Application;
  class Dialog;
  class BorderBase;
  class Label;
  class Icon;
  class CheckBox;
  class PushButton;
  class ToolButton;

  //! Draws standard widgets.
  /*!
   * This class parses a stylesheet and stores palette, images, fonts and various hints.
   * You should override drawing methods in your implementations.
   */
  class Stylist
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
     *
     * @param styleSheet path to a valid style file.
     */
    Stylist();

    /*!
     * Destructor.
     */
    virtual
    ~Stylist();

    /*!
     * Sets the palette using a file.
     * Returns true if successful.
     *
     * @param palette path to a valid palette file.
     */
    bool
    setPaletteFromFile(const char* palette);

    /*!
     * Sets the images/hints/fonts using a file.
     * Returns true if successful.
     *
     * @param style path to a valid style file.
     */
    bool
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
    drawFrame(Painter* painter, BorderBase* frame);

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
     * Draws a pushbutton.
     */
    virtual void
    drawPushButton(Painter* painter, PushButton* button);

    /*!
     * Draws a toolbutton.
     */
    virtual void
    drawToolButton(Painter* painter, ToolButton* button);

    void
    animate(StyledAnimation type, Widget* target);

  protected:
    //! This property stores the palette.
    Palette _palette;
    //! This property stores the style items.
    Style _style;
    //! This property specifies the width of borders around widgets.
    int _borderWidth;

    struct AnimatedElement
    {
      TweenAnimation* in;
      TweenAnimation* out;
      Widget* targetIn;
      Widget* targetOut;
      float valueIn;
      float valueOut;
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
}

#endif /* ILIXI_STYLIST_H_ */
