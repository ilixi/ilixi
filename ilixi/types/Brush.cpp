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

#include <types/Brush.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_BRUSH, "ilixi/types/Brush", "Brush");

Brush::Brush()
        : _modified(true),
          _color(1, 1, 1)
{
    ILOG_TRACE(ILX_BRUSH);
}

Brush::Brush(const Brush& brush)
        : _modified(true),
          _color(brush._color)
{
    ILOG_TRACE(ILX_BRUSH);
}

Brush::Brush(const Color& color)
        : _modified(true),
          _color(color)
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
    }
    return *this;
}

bool
Brush::applyBrush(IDirectFBSurface* surface)
{
    if (_modified)
    {
        DFBResult ret = surface->SetColor(surface, _color.red(), _color.green(),
                                          _color.blue(), _color.alpha());
        if (ret)
        {
            ILOG_ERROR(ILX_BRUSH, "Error while applying brush: %x", ret);
            return false;
        }
        _modified = false;
    }
    return true;
}

} /* namespace ilixi */
