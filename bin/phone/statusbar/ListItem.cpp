/*
 * MenuItem.cpp
 *
 *  Created on: Dec 5, 2012
 *      Author: tarik
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
