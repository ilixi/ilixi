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
          _click(false)
{
    setXConstraint(FixedConstraint);
    setInputMethod(PointerInput);
    if (!bgDef)
        bgDef = new Image(ILIXI_DATADIR"statusbar/statusbar-buttons.png");
}

StatusbarButton::~StatusbarButton()
{
}

Size
StatusbarButton::preferredSize() const
{
    return Size(80, 50);
}

unsigned int
StatusbarButton::state() const
{
    return _myState;
}

void
StatusbarButton::setState(unsigned int state)
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
        y = 54;
    else if (_state & ExposedState)
        y = 108;

    Painter p(this);
    p.begin(event);

    p.blitImage(bgDef, Rectangle(0, y, 15, 15), 0, 0);
    p.blitImage(bgDef, Rectangle(106, y, 20, 15), width() - 20, 0);

    // mid
    p.setClip(15, 0, width() - 35, height());
    p.tileImage(bgDef, 15, 0, Rectangle(15, y, 91, 50));
    p.resetClip();

    // left
    p.setClip(0, 15, 15, height());
    p.tileImage(bgDef, 0, 0, Rectangle(0, y + 15, 15, 39));
    p.resetClip();

    // right
    p.setClip(width() - 20, 15, 20, height());
    p.tileImage(bgDef, width() - 20, 0, Rectangle(106, y + 15, 20, 39));
    p.resetClip();

    p.drawImage(_images[_myState], 15, 0);
    p.end();
}

} /* namespace ilixi */
