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

#include <ui/Button.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_BUTTON, "ilixi/ui/Button", "Button");

Button::Button(const std::string& text, Widget* parent)
        : Widget(parent),
          TextBase("", this),
          _buttonFlag(None)
{
    setText(text);
    setInputMethod(KeyPointer);
    ILOG_TRACE_W(ILX_BUTTON);
}

Button::~Button()
{
    ILOG_TRACE_W(ILX_BUTTON);
}

bool
Button::checkable() const
{
    return (_buttonFlag & Checkable);
}

bool
Button::checked() const
{
    return (_buttonFlag & Checked);
}

void
Button::click(unsigned int ms)
{
    if (ms)
    {
        _state = (WidgetState) (_state | PressedState);
        sigStateChanged(this, _state);
        update();
        usleep(ms * 1000);
        _state = (WidgetState) (_state & ~PressedState);
        sigStateChanged(this, _state);
    }
    sigClicked();
    toggleChecked();
}

void
Button::toggleChecked()
{
    if (checkable())
    {
        if (checked())
            setChecked(false);
        else
            setChecked(true);
        sigCheckChanged(checked());
    }
    update();
}

void
Button::setCheckable(bool checkable)
{
    if (checkable)
        _buttonFlag = (ButtonFlags) (_buttonFlag | Checkable);
    else
        _buttonFlag = (ButtonFlags) (_buttonFlag & ~Checkable);
}

void
Button::setChecked(bool checked)
{
    if (checked)
        _buttonFlag = (ButtonFlags) (_buttonFlag | Checked);
    else
        _buttonFlag = (ButtonFlags) (_buttonFlag & ~Checked);
    update();
}

void
Button::keyUpEvent(const KeyEvent& event)
{
    if (event.keySymbol == DIKS_SPACE || event.keySymbol == DIKS_OK)
        click(100);
}

void
Button::pointerButtonDownEvent(const PointerEvent& event)
{
    _buttonFlag = (ButtonFlags) (_buttonFlag | PressedDown);
    sigPressed();
    update();
}

void
Button::pointerButtonUpEvent(const PointerEvent& event)
{
    sigReleased();
    if (_buttonFlag & PressedDown)
    {
        sigClicked();
        toggleChecked();
        _buttonFlag = (ButtonFlags) (_buttonFlag & ~PressedDown);
    }
}

void
Button::enterEvent(const PointerEvent& event)
{
    update();
}

void
Button::leaveEvent(const PointerEvent& event)
{
    update();
}

void
Button::focusInEvent()
{
    update();
}

void
Button::focusOutEvent()
{
    update();
}

Font*
Button::defaultFont() const
{
    return stylist()->defaultFont(StyleHint::ButtonFont);
}

} /* namespace ilixi */
