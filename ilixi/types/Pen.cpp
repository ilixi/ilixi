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

#include <types/Pen.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_PEN, "ilixi/types/Pen", "Pen");

Pen::Pen()
        : _modified(true),
          _color(0, 0, 0)
{
    ILOG_TRACE(ILX_PEN);
}

Pen::Pen(const Pen& pen)
        : _modified(true),
          _color(pen._color)
{
    ILOG_TRACE(ILX_PEN);
}

Pen::Pen(const Color& color)
        : _modified(true),
          _color(color)
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
        _modified = true;
    }
    return *this;
}

bool
Pen::applyPen(IDirectFBSurface* surface)
{
    if (_modified)
    {
        DFBResult ret = surface->SetColor(surface, _color.red(), _color.green(),
                                          _color.blue(), _color.alpha());
        if (ret)
        {
            ILOG_ERROR(ILX_PEN, "Error while applying brush: %x", ret);
            return false;
        }
        _modified = false;
    }
    return true;
}

} /* namespace ilixi */
