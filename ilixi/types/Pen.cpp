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

#include <types/Pen.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_PEN, "ilixi/types/Pen", "Pen");

Pen::Pen()
        : _modified(true),
          _color(0, 0, 0)
#ifdef ILIXI_HAVE_CAIRO
          ,_mode(SolidColorMode),
          _gradient(),
          _lineWidth(2),
          _lineJoinStyle(MitterJoin),
          _lineCapStyle(ButtCap),
          _dashCount(0),
          _dashOffset(0),
          _dashPattern(NULL)
#endif
{
    ILOG_TRACE(ILX_PEN);
}

Pen::Pen(const Pen& pen)
        : _modified(true),
          _color(pen._color)
#ifdef ILIXI_HAVE_CAIRO
          ,_mode(pen._mode),
          _gradient(pen._gradient),
          _lineWidth(pen._lineWidth),
          _lineJoinStyle(pen._lineJoinStyle),
          _lineCapStyle(pen._lineCapStyle),
          _dashCount(0),
          _dashOffset(0),
          _dashPattern(NULL)
#endif
{
    ILOG_TRACE(ILX_PEN);
#ifdef ILIXI_HAVE_CAIRO
    if (pen.dashPattern())
      setDash(pen.dashPattern(), pen.dashCount(), pen.dashOffset());
#endif
}

Pen::Pen(const Color& color)
        : _modified(true),
          _color(color)
#ifdef ILIXI_HAVE_CAIRO
          ,_mode(SolidColorMode),
          _gradient(),
          _lineWidth(2),
          _lineJoinStyle(MitterJoin),
          _lineCapStyle(ButtCap),
          _dashCount(0),
          _dashOffset(0),
          _dashPattern(NULL)
#endif
{
    ILOG_TRACE(ILX_PEN);
}

Pen::~Pen()
{
    ILOG_TRACE(ILX_PEN);
}

Color
Pen::color() const
{
    return _color;
}

void
Pen::setColor(const Color& color)
{
    _color = color;
    _modified = true;
}

Pen&
Pen::operator=(const Pen& pen)
{
    if (this != &pen)
    {
        _color = pen._color;
#ifdef ILIXI_HAVE_CAIRO
        _mode = pen._mode;
        _lineCapStyle = pen._lineCapStyle;
        _lineJoinStyle = pen._lineJoinStyle;
        _lineWidth = pen._lineWidth;
        if (pen._dashPattern)
          setDash(pen._dashPattern, pen._dashCount, pen._dashOffset);
        else
          clearDashPattern();
#endif
        _modified = true;
    }
    return *this;
}

bool
Pen::applyPen(IDirectFBSurface* surface)
{
    if (_modified)
    {
        DFBResult ret = surface->SetColor(surface, _color.red(), _color.green(), _color.blue(), _color.alpha());
        if (ret)
        {
            ILOG_ERROR(ILX_PEN, "Error while applying brush: %x", ret);
            return false;
        }
        _modified = false;
    }
    return true;
}

#ifdef ILIXI_HAVE_CAIRO
Gradient
Pen::gradient() const
{
    return _gradient;
}

Pen::LineCapStyle
Pen::lineCapStyle() const
{
    return _lineCapStyle;
}

Pen::LineJoinStyle
Pen::lineJoinStyle() const
{
    return _lineJoinStyle;
}

double
Pen::lineWidth() const
{
    return _lineWidth;
}

int
Pen::dashCount() const
{
    return _dashCount;
}

double
Pen::dashOffset() const
{
    return _dashOffset;
}

double*
Pen::dashPattern() const
{
    return _dashPattern;
}

void
Pen::clearDashPattern()
{
    _dashCount = 0;
    _modified = true;
}

void
Pen::setDash(const double* dashes, unsigned int dashCount, double offset)
{
    delete[] _dashPattern;
    _dashOffset = offset;
    _dashCount = dashCount;
    _dashPattern = new double[dashCount];
    for (unsigned int i = 0; i < dashCount; ++i)
        _dashPattern[i] = dashes[i];
    _modified = true;
}

void
Pen::setLineCapStyle(LineCapStyle lineCapStyle)
{
    _lineCapStyle = lineCapStyle;
    _modified = true;
}

void
Pen::setLineJoinStyle(LineJoinStyle lineJoinStyle)
{
    _lineJoinStyle = lineJoinStyle;
    _modified = true;
}

void
Pen::setLineWidth(double width)
{
    /// FIXME clipping is problematic if width is 1
    _lineWidth = width;
    _modified = true;
}

void
Pen::setGradient(const Gradient& gradient)
{
    _gradient = gradient;
    _mode = GradientMode;
    _modified = true;
}

void
Pen::setPenMode(PenMode mode)
{
    _mode = mode;
    _modified = true;
}

void
Pen::applyPen(cairo_t* context)
{
    if (_dashPattern)
        cairo_set_dash(context, _dashPattern, _dashCount, _dashOffset);
    else
        cairo_set_dash(context, 0, 0, 0);

    if (_mode == GradientMode && _gradient.type() != Gradient::None)
        cairo_set_source(context, _gradient.cairoGradient());
    else
        cairo_set_source_rgba(context, _color.red() / 255.0, _color.green() / 255.0, _color.blue() / 255.0, _color.alpha() / 255.0);

    cairo_set_line_width(context, _lineWidth);
    cairo_set_line_join(context, (cairo_line_join_t) _lineJoinStyle);
    cairo_set_line_cap(context, (cairo_line_cap_t) _lineCapStyle);
    _modified = false;
}

#endif

} /* namespace ilixi */
