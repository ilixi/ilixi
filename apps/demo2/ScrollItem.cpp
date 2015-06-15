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

#include "ScrollItem.h"
#include "graphics/Painter.h"
#include "core/Logger.h"

using namespace ilixi;

D_DEBUG_DOMAIN( ILX_SCROLLITEM, "ilixi/ui/ScrollItem", "ScrollItem");

ScrollItem::ScrollItem(const std::string& text, Widget* parent)
        : Widget(parent),
          TextBase("", this),
          _val1(0),
          _image(NULL)
{
    ILOG_TRACE_W(ILX_SCROLLITEM);
    setText(text);
    setConstraints(MinimumConstraint, FixedConstraint);
    setInputMethod(KeyPointer);
}

ScrollItem::~ScrollItem()
{
    ILOG_TRACE_W(ILX_SCROLLITEM);
    delete _image;
}

Size
ScrollItem::preferredSize() const
{
    return Size(200, 50);
}

void
ScrollItem::setImage(Image* image)
{
    _image = image;
}

void
ScrollItem::tweenSlot()
{
    repaint();
}

void
ScrollItem::focusInEvent()
{
    sigFocused(_layout.text());
    update();
}

void
ScrollItem::focusOutEvent()
{
    update();
}

void
ScrollItem::enterEvent(const PointerEvent& event)
{
    update();
}

void
ScrollItem::leaveEvent(const PointerEvent& event)
{
    update();
}

void
ScrollItem::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);

    // draw frame
    Color(51, 128, 204);
    if (state() & FocusedState)
        p.setBrush(Color(51, 128, 204));
    else if (state() & ExposedState)
        p.setBrush(Color(127, 127, 127));
    else
        p.setBrush(Color(255, 255, 255));
    p.fillRectangle(0, 0, width(), height());

    p.stretchImage(_image, 0, 0, 50, 50);

    p.setBrush(Color(0, 0, 0));
    p.drawLayout(_layout, 55, 0);

    p.end();
}

void
ScrollItem::updateTextBaseGeometry()
{
    _layout.setBounds(0, 0, width() - 30, height());
    _layout.doLayout(font());
}
