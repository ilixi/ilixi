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

#include <ui/Spacer.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_SPACER, "ilixi/ui/Spacer", "Spacer");

Spacer::Spacer(Orientation orientation, Widget* parent)
        : Widget(parent)
{
    setOrientation(orientation);
}

Spacer::~Spacer()
{
}

Size
Spacer::preferredSize() const
{
    return Size(1, 1);
}

void
Spacer::setOrientation(Orientation orientation)
{
    if (orientation == Horizontal)
        setConstraints(ExpandingConstraint, FixedConstraint);
    else
        setConstraints(FixedConstraint, ExpandingConstraint);
}

void
Spacer::paint(const PaintEvent& event)
{
}

void
Spacer::compose(const PaintEvent& event)
{
}

}
