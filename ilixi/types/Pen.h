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

#ifndef ILIXI_PEN_H_
#define ILIXI_PEN_H_

#include <types/Color.h>

namespace ilixi
{
//! Defines color for drawing outlines of primitive shapes.
/*!
 * Pen is used by painter during drawing.
 *
 * By default, a pen only applies a solid color.
 */
class Pen
{
    friend class Painter; // applyPen()
public:
    /*!
     * Creates an opaque black pen.
     */
    Pen();

    /*!
     * Copy constructor.
     */
    Pen(const Pen& pen);

    /*!
     * Creates a brush using the given color.
     */
    Pen(const Color & color);

    /*!
     * Destructor.
     */
    ~Pen();

    /*!
     * Returns current color.
     */
    Color
    color() const;

    /*!
     * Sets the pen color to the given color.
     */
    void
    setColor(const Color& color);

    /*!
     * Assignment operator.
     */
    Pen&
    operator=(const Pen &pen);

private:
    //! Flag is set to true if pen is modified.
    bool _modified;
    //! This property holds current pen color.
    Color _color;

    //! Applies pen to surface.
    bool
    applyPen(IDirectFBSurface* surface);
};
}

#endif /* ILIXI_PEN_H_ */
