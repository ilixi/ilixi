/*
 * Contact.cpp
 *
 *  Created on: Jul 1, 2013
 *      Author: tarik
 */

#include "Contact.h"

#include <core/Logger.h>
#include <core/PlatformManager.h>
#include <graphics/Painter.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_SIP_CONTACT, "ilixi_sip/Contact", "Contact");

Contact::Contact(const std::string& name, const std::string& addr, Widget* parent)
        : Widget(parent),
          _selected(false),
          _pressed(false),
          _status(Online),
          _name(name),
          _addr(addr)
{
    // TODO status should be Offline at start
    setConstraints(MinimumConstraint, FixedConstraint);
    setInputMethod(KeyPointer);
}

Contact::~Contact()
{
}

Size
Contact::preferredSize() const
{
    return Size(200, 30);
}

const std::string&
Contact::addr() const
{
    return _addr;
}

void
Contact::setAddr(const std::string& addr)
{
    _addr = addr;
}

const std::string&
Contact::name() const
{
    return _name;
}

void
Contact::setName(const std::string& name)
{
    _name = name;
}

Contact::ContactStatus
Contact::status() const
{
    return _status;
}

void
Contact::setStatus(ContactStatus status)
{
    _status = status;
}

void
Contact::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);

    if (_selected)
    {
        p.setBrush(stylist()->palette()->focus);
        p.fillRectangle(15, 0, width(), height());
    }

    if (_status == Online)
        p.setBrush(Color("238B49"));
    else if (_status == Busy)
        p.setBrush(Color("BF3330"));
    else if (_status == Away)
        p.setBrush(Color("26222B"));
    else
        p.setBrush(Color("26222B"));
    p.fillRectangle(0, 0, 15, 30);

    p.setFont(*stylist()->defaultFont(StyleHint::ButtonFont));
    if (_status == Offline)
        p.setBrush(stylist()->palette()->getGroup(DisabledState).text);
    else
        p.setBrush(stylist()->palette()->getGroup(state()).text);

    p.drawText(_name, 20, 5);
}

void
Contact::keyDownEvent(const KeyEvent& event)
{
    if (event.keySymbol == DIKS_SPACE || event.keySymbol == DIKS_OK)
        sigClicked();
}

void
Contact::pointerButtonDownEvent(const PointerEvent& event)
{
    _pressed = true;
    update();
}

void
Contact::pointerButtonUpEvent(const PointerEvent& event)
{
    if (_pressed)
    {
        PlatformManager::instance().playSoundEffect("Click");
        _pressed = false;
        update();
        sigClicked();
    }
}

void
Contact::enterEvent(const PointerEvent& event)
{
    update();
}

void
Contact::leaveEvent(const PointerEvent& event)
{
    update();
}

void
Contact::focusInEvent()
{
    _selected = true;
    update();
}

void
Contact::setSelected(bool selected)
{
    _selected = selected;
    update();
}

void
Contact::focusOutEvent()
{
    update();
}

} /* namespace ilixi */
