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

#include <types/Margin.h>

namespace ilixi
{

Margin::Margin()
        : _top(0),
          _bottom(0),
          _left(0),
          _right(0)
{
}

Margin::Margin(int margin)
        : _top(margin),
          _bottom(margin),
          _left(margin),
          _right(margin)
{
}

Margin::Margin(int top, int bottom, int left, int right)
        : _top(top),
          _bottom(bottom),
          _left(left),
          _right(right)
{
}

Margin::Margin(const Margin& margin)
        : _top(margin._top),
          _bottom(margin._bottom),
          _left(margin._left),
          _right(margin._right)
{
}

Margin::~Margin()
{
}

int
Margin::bottom() const
{
    return _bottom;
}

int
Margin::left() const
{
    return _left;
}

int
Margin::right() const
{
    return _right;
}

int
Margin::top() const
{
    return _top;
}

int
Margin::hSum() const
{
    return _left + _right;
}

int
Margin::vSum() const
{
    return _bottom + _top;
}

void
Margin::setBottom(int bottom)
{
    _bottom = bottom;
}

void
Margin::setLeft(int left)
{
    _left = left;
}

void
Margin::setRight(int right)
{
    _right = right;
}

void
Margin::setTop(int top)
{
    _top = top;
}

void
Margin::setMargins(int top, int bottom, int left, int right)
{
    _top = top;
    _bottom = bottom;
    _left = left;
    _right = right;
}

bool
Margin::operator==(const Margin &margin)
{
    return ((_top == margin.top()) && (_bottom == margin.bottom())
            && (_left == margin.left()) && (_right == margin.right()));
}

bool
Margin::operator!=(const Margin &margin)
{
    return !(*this == margin);
}

Margin&
Margin::operator=(const Margin& m)
{
    if (this != &m)
    {
        _top = m._top;
        _bottom = m._bottom;
        _left = m._left;
        _right = m._right;
    }
    return *this;
}

} /* namespace ilixi */
