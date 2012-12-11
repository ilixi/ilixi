/*
 Copyright 2010-2012 Tarik Sekmen.

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

#include "NotificationIcon.h"
#include <graphics/Painter.h>

namespace ilixi
{

NotificationIcon::NotificationIcon(Widget* parent)
        : Widget(parent),
          _state(-1)
{
}

NotificationIcon::~NotificationIcon()
{
    for (StateVector::iterator it = _states.begin(); it != _states.end(); ++it)
        delete *it;
}

Size
NotificationIcon::preferredSize() const
{
    return Size(32, 32);
}

void
NotificationIcon::addState(const std::string& icon)
{
    _state = _states.size();
    _states.push_back(new Image(icon));
}

unsigned int
NotificationIcon::state() const
{
    return _state;
}

void
NotificationIcon::setState(unsigned int state)
{
    if (state < _states.size())
    {
        _state = state;
        update();
    }
}

void
NotificationIcon::compose(const PaintEvent& event)
{
    if (_state == -1)
        return;

    Painter p(this);
    p.begin(event);
    p.drawImage(_states[_state], 0, 0);
}

} /* namespace ilixi */
