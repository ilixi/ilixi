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

#ifndef ILIXI_PALETTE_H_
#define ILIXI_PALETTE_H_

#include "types/Enums.h"
#include "types/Color.h"

namespace ilixi
{
  //! Group of colours for widget states.
  struct ColorGroup
  {
    //! Initialise members to default colours.
    ColorGroup();

    //! Copy
    ColorGroup(const ColorGroup& c);

    //! Initialise members using given colours.
    ColorGroup(Color base, Color baseAlt, Color backgroundTop,
        Color backgrounMid, Color backgrounBottom, Color borderTop,
        Color borderMid, Color borderBottom, Color text);

    //! Set all colours.
    void
    setColors(Color base, Color baseAlt, Color backgroundTop,
        Color backgrounMid, Color backgrounBottom, Color borderTop,
        Color borderMid, Color borderBottom, Color text);

    //! Base colour; use as the background colour of an input widget or row colour inside a list widget.
    Color base;
    //! Text colour; use for drawing text on base.
    Color baseText;
    //! Alternative base colour; use for lists.
    Color baseAlt;
    //! Text colour; use for drawing text on alternative base.
    Color baseAltText;

    //! Alternative background colour; use with gradients.
    Color bgBottom;
    //! Alternative background colour; use with gradients.
    Color bgMid;
    //! Primary background colour.
    Color bgTop;

    //! Alternative border colour; use with gradients.
    Color borderBottom;
    //! Alternative border colour; use with gradients.
    Color borderMid;
    //! Primary border colour.
    Color borderTop;

    //! Alternative fill colour; used with gradients.
    Color fillBottom;
    //! Alternative fill colour; use with gradients.
    Color fillMid;
    //! Primary colour use for filling a region inside widget.
    Color fillTop;

    //! Text colour; use for drawing text or symbols inside a widget.
    Color text;
  };

  //! Stores colours for drawing.
  /*!
   * A palette is a collection of colors which are used by a Stylist
   * during drawing widgets in various states.
   *
   * All fields are made public for ease of access and speed.
   */
  struct Palette
  {
  public:
    /*!
     * Initialise to default.
     */
    Palette();

    /*!
     * Returns colour group given a state.
     */
    ColorGroup&
    getGroup(WidgetState state);

    /*!
     * Initialise palette from an XML file.
     *
     * Returns false on error.
     */
    bool
    parsePalette(const char* file);

    //! Alternative window background colour; use with gradients.
    Color bgBottom;
    //! Alternative window background colour; use with gradients.
    Color bgMid;
    //! Primary window background colour.
    Color bgTop;

    //! Alternative colour for drawing focus rectangles; use with gradients.
    Color focusBottom;
    //! Alternative colour for drawing focus rectangles; use with gradients.
    Color focusMid;
    //! Primary colour for drawing focus rectangles.
    Color focusTop;

    //! Use for drawing text on containers with no background.
    Color text;
    //! Use for drawing disabled text on containers with no background.
    Color textDisabled;

    //! Default state.
    ColorGroup _default;
    //! Exposed state.
    ColorGroup _exposed;
    //! Pressed state.
    ColorGroup _pressed;
    //! Disabled state.
    ColorGroup _disabled;

  };
}

#endif /* ILIXI_PALETTE_H_ */
