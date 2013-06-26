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

#include "StatusbarButton.h"
#include <graphics/Painter.h>

namespace ilixi
{

Image* StatusbarButton::bgDef = NULL;

StatusbarButton::StatusbarButton(Widget* parent)
        : Widget(parent),
          _myState(0),
          _click(false),
          _active(false)
{
    setXConstraint(FixedConstraint);
    setInputMethod(PointerInput);
    if (!bgDef)
        bgDef = new Image(ILIXI_DATADIR"car/statusbar/statusbar-buttons.png");
}

StatusbarButton::~StatusbarButton()
{
}

Size
StatusbarButton::preferredSize() const
{
    return Size(75, 50);
}

bool
StatusbarButton::active() const
{
    return _active;
}

unsigned int
StatusbarButton::buttonState() const
{
    return _myState + _active;
}

void
StatusbarButton::setActive(bool active)
{
    _active = active;
    update();
}

void
StatusbarButton::setButtonState(unsigned int state)
{
    if (state != _myState)
    {
        _myState = state;
        update();
    }
}

void
StatusbarButton::addImage(Image* image)
{
    _images.push_back(image);
}

void
StatusbarButton::pointerButtonDownEvent(const PointerEvent& pointerEvent)
{
    _click = true;
    update();
}

void
StatusbarButton::pointerButtonUpEvent(const PointerEvent& pointerEvent)
{
    if (_click)
    {
        sigClicked();
        _click = false;
        update();
    }
}

void
StatusbarButton::enterEvent(const PointerEvent& event)
{
    update();
}

void
StatusbarButton::leaveEvent(const PointerEvent& event)
{
    update();
}

void
StatusbarButton::compose(const PaintEvent& event)
{
    int y = 0;
    if (_state & PressedState)
        y = 55;
    else if (_state & ExposedState)
        y = 110;

    Painter p(this);
    p.begin(event);
    p.blitImage(bgDef, Rectangle(0, y, 15, 55), 0, 0);
    p.blitImage(bgDef, Rectangle(106, y, 20, 55), width() - 20, 0);

    p.stretchImage(bgDef, Rectangle(15, 0, width() - 35, 55), Rectangle(15, y, 90, 55)); // mid

    p.drawImage(_images[buttonState()], (width() - 48) / 2, (height() - 48) / 2);
    p.end();
}

} /* namespace ilixi */
