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

#ifndef ILIXI_EVENTS_H_
#define ILIXI_EVENTS_H_

#include "directfb.h"
#include "types/Rectangle.h"

namespace ilixi
{
  enum KeyEventType
  {
    KeyUpEvent, KeyDownEvent
  };

  struct KeyEvent
  {
    KeyEvent(KeyEventType type, DFBInputDeviceKeySymbol symbol) :
      eventType(type), keySymbol(symbol)
    {
    }

    KeyEvent(KeyEventType type, DFBInputDeviceKeySymbol symbol,
        DFBInputDeviceModifierMask mask, DFBInputDeviceLockState locks) :
      eventType(type), keySymbol(symbol), modifierMask(mask), lockState(locks)
    {
    }

    DFBInputDeviceKeySymbol keySymbol;
    DFBInputDeviceModifierMask modifierMask;
    DFBInputDeviceLockState lockState;
    KeyEventType eventType;
  };

  enum PointerEventType
  {
    PointerButtonDown, PointerButtonUp, PointerWheel, PointerMotion
  };

  enum PointerButton
  {
    ButtonLeft = 0x00000000,
    ButtonRight = 0x00000001,
    ButtonMiddle = 0x00000002,
    ButtonNone = 0x00000020
  };

  enum PointerButtonMask
  {
    ButtonMaskNone = 0,
    ButtonMaskLeft = 0x00000001,
    ButtonMaskRight = 0x00000002,
    ButtonMaskMiddle = 0x00000004
  };

  struct PointerEvent
  {
    PointerEvent() :
      eventType(PointerMotion), x(0), y(0), wheelStep(0), button(ButtonNone),
          buttonMask(ButtonMaskNone), timestamp(direct_clock_get_millis())
    {
    }

    PointerEvent(PointerEventType type, int X, int Y, int step = 0,
        PointerButton pbutton = ButtonNone,
        PointerButtonMask mask = ButtonMaskNone) :
      eventType(type), x(X), y(Y), wheelStep(step), button(pbutton),
          buttonMask(mask), timestamp(direct_clock_get_millis())
    {
    }

    PointerEventType eventType;
    int x;
    int y;
    int wheelStep;
    PointerButton button;
    PointerButtonMask buttonMask;
    long timestamp;
  };
}

#endif /* ILIXI_EVENTS_H_ */
