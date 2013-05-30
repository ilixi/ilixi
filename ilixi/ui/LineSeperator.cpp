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

#include <ui/LineSeperator.h>
#include <graphics/Painter.h>
#include <core/Logger.h>
namespace ilixi
{

LineSeperator::LineSeperator(Orientation orientation, Widget* parent)
        : Widget(parent),
          _orientation(orientation)
{
    setOrientation(orientation);
}

LineSeperator::~LineSeperator()
{
}

Size
LineSeperator::preferredSize() const
{
    if (_orientation == Horizontal)
        return Size(100, stylist()->defaultParameter(StyleHint::LineSeperatorHeight));
    return Size(stylist()->defaultParameter(StyleHint::LineSeperatorWidth), 100);
}

Orientation
LineSeperator::orientation() const
{
    return _orientation;
}

void
LineSeperator::setOrientation(Orientation orientation)
{
    _orientation = orientation;
    if (_orientation == Horizontal)
        setConstraints(ExpandingConstraint, FixedConstraint);
    else
        setConstraints(FixedConstraint, ExpandingConstraint);
}

void
LineSeperator::compose(const ilixi::PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    stylist()->drawLineSeperator(&p, this);
}

} /* namespace ilixi */
