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

#ifndef ILIXI_PEN_H_
#define ILIXI_PEN_H_

#include <ilixiConfig.h>
#include <types/Color.h>

#ifdef ILIXI_HAVE_CAIRO
#include <types/Gradient.h>
#endif

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
    friend class CairoPainter; // applyBrush()
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

#ifdef ILIXI_HAVE_CAIRO
    /*!
     * Available pen modes.
     */
    enum PenMode
    {
        SolidColorMode, //!< Shapes are stroked using a solid color.
        GradientMode//!< Shapes are stroked using a gradient
    };

    /*!
     * This enum specifies how to render the junction of two lines when stroking.
     */
    enum LineJoinStyle
    {
        MitterJoin, //!< Use a sharp corner.
        RoundJoin,//!< Use a rounded join, the center of the circle is the joint point.
        BevelJoin//!< Use a cut-off join, the join is cut off at half the line width from the joint point.
    };

    /*!
     * This enum specifies how to render the endpoints of the path when stroking.
     */
    enum LineCapStyle
    {
        ButtCap,    //!< Start(stop) the line exactly at the start(end) point
        RoundCap,//!< Use a round ending, the center of the circle is the end point
        SquareCap//!< Use squared ending, the center of the square is the end point
    };

    /*!
     * Returns current gradient.
     */
    Gradient
    gradient() const;

    /*!
     * Returns current line join style.
     */
    LineJoinStyle
    lineJoinStyle() const;

    /*!
     * Returns current line cap style.
     */
    LineCapStyle
    lineCapStyle() const;

    /*!
     * Returns current line width.
     */
    double
    lineWidth() const;

    /*!
     * Returns number of dashes in current dash pattern.
     */
    int
    dashCount() const;

    /*!
     * Returns offset for dash pattern.
     */
    double
    dashOffset() const;

    /*!
     * Returns current dash pattern in use.
     */
    double*
    dashPattern() const;

    /*!
     * Clears dash pattern.
     */
    void
    clearDashPattern();

    /*!
     * Sets the dash pattern used by pen. A dash pattern is specified by dashes, an array of
     * positive values. Each value provides the length of alternate "on" and "off" portions
     * of the stroke. The offset specifies an offset into the pattern at which the stroke begins.
     *
     * If dashCount is 0 then dashing is disabled.
     * If dashCount is 1 a symmetric pattern is assumed with alternating on and off
     * portions of the size specified by the single value in dashes.
     *
     * @param dashes An array specifying alternate lengths of on and off stroke portions
     * @param dashCount The length of the dashes array
     * @param offset An offset into the dash pattern at which the stroke should start
     */
    void
    setDash(const double *dashes, unsigned int dashCount, double offset);

    /*!
     * Sets the line cap style. Default is ButtCap.
     *
     * @param lineCapStyle
     */
    void
    setLineCapStyle(LineCapStyle lineCapStyle);

    /*!
     * Sets the line join style. Default is MitterJoin.
     *
     * @param lineJoinStyle
     */
    void
    setLineJoinStyle(LineJoinStyle lineJoinStyle);

    /*!
     * Sets the current line width. Default is 1.
     */
    void
    setLineWidth(double width);

    /*!
     * The given gradient is assigned to the brush's current gradient.
     * Reference to previous gradient (cairo_pattern_t) is decremented by 1 internally.
     *
     * \code
     * Pen pen;
     * RadialGradient radialGrad(50, 50, 10, 50, 50, 100);
     * radialGrad(Color(0, 0, 0), 0.0);
     * radialGrad(Color(0, 1, 0), 1);
     * pen.setGradient(&radialGrad);
     * \endcode
     */
    void
    setGradient(const Gradient& gradient);

    /*!
     * Sets the pen's mode.
     *
     * If there is no gradient available for the pen, mode falls back to SolidColor.
     */
    void
    setPenMode(PenMode mode);
#endif

private:
    //! Flag is set to true if pen is modified.
    bool _modified;
    //! This property holds current pen color.
    Color _color;

#ifdef ILIXI_HAVE_CAIRO
    //! This property holds current pen mode.
    PenMode _mode;
    //! This property holds gradient used by the pen.
    Gradient _gradient;
    //! This property holds current line width.
    double _lineWidth;
    //! This property holds current line join style.
    LineJoinStyle _lineJoinStyle;
    //! This property holds current line cap style.
    LineCapStyle _lineCapStyle;
    //! This property holds dash counts in current dash pattern.
    unsigned int _dashCount;
    //! This property holds offset into the dash pattern at which the stroke should start.
    double _dashOffset;
    //! This property holds current dash pattern.
    double* _dashPattern;

    //! Applies the pen to the cairo context.
    void
    applyPen(cairo_t* context);
#endif

    //! Applies pen to surface.
    bool
    applyPen(IDirectFBSurface* surface);
};
}

#endif /* ILIXI_PEN_H_ */
