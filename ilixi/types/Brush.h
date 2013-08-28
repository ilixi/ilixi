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

#ifndef ILIXI_BRUSH_H_
#define ILIXI_BRUSH_H_

#include <types/Color.h>

namespace ilixi
{
//! Defines color for filling inside primitive shapes.
/*!
 * Brush is used by painter during drawing.
 *
 * By default, a brush only applies a solid color.
 */
class Brush
{
    friend class Painter; // applyBrush()
public:
    /*!
     * Creates an opaque white brush.
     */
    Brush();

    /*!
     * Copy constructor.
     */
    Brush(const Brush& brush);

    /*!
     * Creates a brush using the given color.
     */
    Brush(const Color& color);

    /*!
     * Destructor.
     */
    ~Brush();

    /*!
     * Returns current color.
     */
    Color
    color() const;

    /*!
     * Sets the brush color to the given color.
     */
    void
    setColor(const Color& color);

    /*!
     * Assignment operator.
     */
    Brush&
    operator=(const Brush &brush);

private:
    //! Flag is set to true if pen is modified.
    bool _modified;
    //! This property holds current brush color.
    Color _color;

    //! Applies brush to surface.
    bool
    applyBrush(IDirectFBSurface* surface);
};
}

#endif /* ILIXI_BRUSH_H_ */
