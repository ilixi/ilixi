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

#include <types/Size.h>

namespace ilixi
{

Size::Size()
        : _width(-1),
          _height(-1)
{
}

Size::Size(const Size& size)
        : _width(size._width),
          _height(size._height)
{
}

Size::Size(int width, int height)
        : _width(width),
          _height(height)
{
}

Size::~Size()
{
}

bool
Size::isValid() const
{
    if (_width <= 0 || _height <= 0)
        return false;
    return true;
}

void
Size::transpose()
{
    int temp = _width;
    _width = _height;
    _height = temp;
}

void
Size::setHeight(int height)
{
    _height = height;
}

void
Size::setWidth(int width)
{
    _width = width;
}

Size&
Size::operator=(const Size &size)
{
    if (this != &size)
    {
        _width = size._width;
        _height = size._height;
    }
    return *this;
}

bool
Size::operator==(const Size &size) const
{
    return ((_width == size._width) && (_height == size._height));
}

bool
Size::operator!=(const Size &size) const
{
    return !(*this == size);
}

} /* namespace ilixi */
