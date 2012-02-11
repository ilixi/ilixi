/*
 Copyright 2011 Tarik Sekmen.

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

#include "ui/Button.h"

using namespace ilixi;

Button::Button(const std::string& text, Widget* parent) :
    TextBase(text, parent), _buttonFlag(None), _icon(NULL)
{
  setInputMethod(KeyAndPointerInput);
}

Button::~Button()
{
  delete _icon;
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
      _state = (WidgetState) (_state ^ PressedState);
      repaint();
      usleep(ms * 1000);
      _state = (WidgetState) (_state ^ PressedState);
    }
  sigClicked();
  toggleChecked();
}

const Icon*
Button::icon() const
{
  return _icon;
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
}

void
Button::setIcon(Icon* icon)
{
  removeChild(_icon);
  _icon = icon;
  addChild(_icon);
}

void
Button::setIcon(const std::string& iconPath, const Size& size)
{
  removeChild(_icon);
  _icon = new Icon(iconPath, this);
  setIconSize(size);
  addChild(_icon);
}

void
Button::setIconSize(const Size& size)
{
  if (_icon)
    {
      if (size.isValid())
        _icon->setSize(size);
      else
        _icon->setSize(_icon->preferredSize());
    }
}

void
Button::keyUpEvent(const KeyEvent& event)
{
  if (event.keySymbol == DIKS_SPACE)
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
