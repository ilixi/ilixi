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

#include "ListItem.h"
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_LISTITEM, "ilixi/pstatus/ListItem", "ListItem");

ListItem::ListItem(const std::string& title, Widget* parent)
        : ToolButton(title, parent)
{
    ILOG_TRACE_W(ILX_LISTITEM);
    setDrawFrame(false);
    _layout.setSingleLine(false);
    _layout.setAlignment(TextLayout::Left);
}

ListItem::~ListItem()
{
}

void
ListItem::setBg(Image* bg)
{
    _bg = bg;
}

Size
ListItem::preferredSize() const
{
    return Size(120, 51);
}

void
ListItem::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    if (state() & FocusedState)
        p.drawImage(_bg, 0, 0);

    p.setFont(*font());
    p.setBrush(Color(255, 255, 255));
    p.drawLayout(_layout, 0, state() & PressedState ? 1 : 0);
}

void
ListItem::focusInEvent()
{
    ILOG_TRACE_W(ILX_LISTITEM);
    update();
}

void
ListItem::focusOutEvent()
{
    ILOG_TRACE_W(ILX_LISTITEM);
    update();
}

}
