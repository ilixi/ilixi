/*
 * HistoryItem.cpp
 *
 *  Created on: Jul 1, 2013
 *      Author: tarik
 */

#include "HistoryItem.h"

#include <core/Logger.h>
#include <graphics/Painter.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_SIP_HISTORYITEM, "ilixi_sip/HistoryItem", "HistoryItem");

HistoryItem::HistoryItem(bool incoming, const std::string& name, const std::string& addr, Widget* parent)
        : Widget(parent),
          _incoming(incoming),
          _selected(false),
          _name(name),
          _addr(addr)
{
    setConstraints(MinimumConstraint, FixedConstraint);
    setInputMethod(KeyPointer);

    if (incoming)
        _image = stylist()->defaultIcon(StyleHint::Left);
    else
        _image = stylist()->defaultIcon(StyleHint::Right);

    struct timeval tv;
    struct tm* tm;
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    _timestamp = PrintF("%02d:%02d - %d/%d/%d", tm->tm_hour, tm->tm_min, tm->tm_mday, tm->tm_mon, tm->tm_year + 1900);
}

HistoryItem::~HistoryItem()
{
    delete _image;
}

Size
HistoryItem::preferredSize() const
{
    return Size(200, 50);
}

const std::string&
HistoryItem::addr() const
{
    return _addr;
}

void
HistoryItem::setAddr(const std::string& addr)
{
    _addr = addr;
}

const std::string&
HistoryItem::name() const
{
    return _name;
}

void
HistoryItem::setName(const std::string& name)
{
    _name = name;
}

const std::string&
HistoryItem::timestamp() const
{
    return _timestamp;
}

void
HistoryItem::setTimestamp(const std::string& timestamp)
{
    _timestamp = timestamp;
}

bool
HistoryItem::isselected() const
{
    return _selected;
}

void
HistoryItem::setSelected(bool selected)
{
    _selected = selected;
    update();
}

void
HistoryItem::focusInEvent()
{
    _selected = true;
    update();
}

void
HistoryItem::focusOutEvent()
{
    update();
}

void
HistoryItem::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);

    if (_selected)
    {
        p.setBrush(stylist()->palette()->focus);
        p.fillRectangle(0, 0, width(), height());
    }

    if (_incoming)
    {
        p.setBrush(Color(0, 255, 0));
        p.drawImage(_image, 0, (height() - 16) / 2, (DFBSurfaceBlittingFlags) (DSBLIT_COLORIZE | DSBLIT_BLEND_ALPHACHANNEL));
    } else
    {
        p.setBrush(Color(255, 0, 0));
        p.drawImage(_image, 0, (height() - 16) / 2, (DFBSurfaceBlittingFlags) (DSBLIT_COLORIZE | DSBLIT_BLEND_ALPHACHANNEL));
    }

    p.setBrush(stylist()->palette()->getGroup(state()).text);
    p.drawText(_timestamp, 25, 30);
    p.setFont(*stylist()->defaultFont(StyleHint::ButtonFont));
    p.drawText(_name, 25, 5);
}

} /* namespace ilixi */
