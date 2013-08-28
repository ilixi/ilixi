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

#ifndef ILIXI_PALETTE_H_
#define ILIXI_PALETTE_H_

#include <types/Enums.h>
#include <types/Color.h>

namespace ilixi
{
//! Group of colours for widget states.
struct ColorGroup
{
    //! Initialise members to default color.
    ColorGroup();

    //! Copy
    ColorGroup(const ColorGroup& c);

    //! Initialise members using given color.
    ColorGroup(Color base, Color baseAlt, Color background, Color border,
               Color fill, Color text);

    //! Set all color.
    void
    setColors(Color base, Color baseAlt, Color background, Color border,
              Color fill, Color text);

    //! Base color; use as the background color of an input widget or row color inside a list widget.
    Color base;
    //! Text color; use for drawing text on base.
    Color baseText;
    //! Alternative base color; use for lists.
    Color baseAlt;
    //! Text color; use for drawing text on alternative base.
    Color baseAltText;
    //! Background color; use with gradients.
    Color bg;
    //! Alternative border color; use with gradients.
    Color border;
    //! Indicator fill color.
    Color fill;
    //! Text colour; use for drawing text or symbols inside a widget.
    Color text;
};

//! Stores color for drawing.
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

    //! Window background color; use with gradients.
    Color bg;
    //! Use for drawing focus rectangles.
    Color focus;
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
