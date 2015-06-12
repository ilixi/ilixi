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

#include <types/Brush.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_BRUSH, "ilixi/types/Brush", "Brush");

Brush::Brush()
        : _modified(true),
          _color(1, 1, 1)
#ifdef ILIXI_HAVE_CAIRO
          ,_mode(SolidColorMode),
          _gradient()
#endif
{
    ILOG_TRACE(ILX_BRUSH);
}

Brush::Brush(const Brush& brush)
        : _modified(true),
          _color(brush._color)
#ifdef ILIXI_HAVE_CAIRO
          ,_mode(brush.mode()),
          _gradient(brush.gradient())
#endif
{
    ILOG_TRACE(ILX_BRUSH);
}

Brush::Brush(const Color& color)
        : _modified(true),
          _color(color)
#ifdef ILIXI_HAVE_CAIRO
          ,_mode(SolidColorMode),
          _gradient()
#endif
{
    ILOG_TRACE(ILX_BRUSH);
}

Brush::~Brush()
{
    ILOG_TRACE(ILX_BRUSH);
}

Color
Brush::color() const
{
    return _color;
}

void
Brush::setColor(const Color& color)
{
    _color = color;
    _modified = true;
}

Brush&
Brush::operator=(const Brush &brush)
{
    if (this != &brush)
    {
        _color = brush._color;
        _modified = true;
#ifdef ILIXI_HAVE_CAIRO
        _mode = brush.mode();
        _gradient = brush.gradient();
#endif
    }
    return *this;
}

bool
Brush::applyBrush(IDirectFBSurface* surface)
{
    if (_modified)
    {
        DFBResult ret = surface->SetColor(surface, _color.red(), _color.green(), _color.blue(), _color.alpha());
        if (ret)
        {
            ILOG_ERROR(ILX_BRUSH, "Error while applying brush: %x", ret);
            return false;
        }
        _modified = false;
    }
    return true;
}

#ifdef ILIXI_HAVE_CAIRO
Brush::BrushMode
Brush::mode() const
{
    return _mode;
}

Gradient
Brush::gradient() const
{
    return _gradient;
}

void
Brush::setBrushMode(BrushMode mode)
{
    _mode = mode;
    _modified = true;
}

void
Brush::setGradient(const Gradient& gradient)
{
    _gradient = gradient;
    _mode = GradientMode;
    _modified = true;
}

bool
Brush::applyBrush(cairo_t* context)
{
    // TODO add error checking
    switch (_mode)
    {
    case GradientMode:
        if (_gradient.type() != Gradient::None)
            cairo_set_source(context, _gradient.cairoGradient());
        break;
    case SolidColorMode:
        cairo_set_source_rgba(context, _color.red() / 255.0, _color.green() / 255.0, _color.blue() / 255.0, _color.alpha() / 255.0);
        break;
    case TextureMode:
        break;
    default:
        break;
    }
    _modified = false;
    return true;
}
#endif

} /* namespace ilixi */
