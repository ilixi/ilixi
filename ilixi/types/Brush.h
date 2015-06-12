/*
 Copyright 2010-2015 Tarik Sekmen.

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

#include <ilixiConfig.h>
#include <types/Color.h>

#ifdef ILIXI_HAVE_CAIRO
#include <types/Gradient.h>
#endif

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
    friend class CairoPainter; // applyBrush()
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

#ifdef ILIXI_HAVE_CAIRO
    /*!
     * Available brush modes.
     */
    enum BrushMode
    {
        None,             //!< Shapes are not filled.
        SolidColorMode,   //!< Shapes are filled using a solid color.
        GradientMode,     //!< Shapes are filled using a gradient
        TextureMode       //!< Shapes are filled using a texture (Not yet implemented!)
    };

    /*!
     * Returns brush mode. Default is SolidColorMode.
     */
    BrushMode
    mode() const;

    /*!
     * Returns current gradient.
     *
     * \sa Gradient::type()
     */
    Gradient
    gradient() const;

    /*!
     * Sets the brush's mode.
     * If there is no gradient available for the brush, mode falls back to SolidColor.
     *
     * @param mode Brush mode.
     */
    void
    setBrushMode(BrushMode mode);

    /*!
     * The given gradient is assigned to the brush's current gradient.
     * Reference to previous gradient (cairo_pattern_t) is decremented by 1 internally.
     *
     * \code
     * Brush b;
     * RadialGradient rGrad(50, 50, 10, 50, 50, 100);
     * rGrad->addStop(Color(0, 0, 0), 0.0);
     * rGrad->addStop(Color(0, 1, 0), 1);
     * b.setGradient(rGrad);
     * \endcode
     *
     * @param gradient
     */
    void
    setGradient(const Gradient& gradient);
#endif

private:
    //! Flag is set to true if pen is modified.
    bool _modified;
    //! This property holds current brush color.
    Color _color;

#ifdef ILIXI_HAVE_CAIRO
    //! This property holds current brush mode.
    BrushMode _mode;
    //! This property holds gradient used by the brush.
    Gradient _gradient;

    //! Applies the brush to the cairo context.
    bool
    applyBrush(cairo_t* context);
#endif

    //! Applies brush to surface.
    bool
    applyBrush(IDirectFBSurface* surface);
};
}

#endif /* ILIXI_BRUSH_H_ */
