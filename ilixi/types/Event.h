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

#ifndef ILIXI_EVENTS_H_
#define ILIXI_EVENTS_H_

#include <directfb.h>
#include <types/Rectangle.h>
#include <ilixiConfig.h>

namespace ilixi
{
enum KeyEventType
{
    KeyDownEvent = 0x00000100, KeyUpEvent = 0x00000200
};

struct KeyEvent
{
    KeyEvent(KeyEventType type, DFBInputDeviceKeySymbol symbol)
            : eventType(type),
              keySymbol(symbol)
    {
    }

    KeyEvent(KeyEventType type, DFBInputDeviceKeySymbol symbol,
             DFBInputDeviceModifierMask mask, DFBInputDeviceLockState locks)
            : eventType(type),
              keySymbol(symbol),
              modifierMask(mask),
              lockState(locks)
    {
    }

    KeyEventType eventType;
    DFBInputDeviceKeySymbol keySymbol;
    DFBInputDeviceModifierMask modifierMask;
    DFBInputDeviceLockState lockState;
};

enum PointerEventType
{
    PointerButtonDown = 0x00010000,
    PointerButtonUp = 0x00020000,
    PointerMotion = 0x00040000,
    PointerWheel = 0x00200000
};

enum PointerButton
{
    ButtonLeft = 0x00000000,
    ButtonRight = 0x00000001,
    ButtonMiddle = 0x00000002,
    ButtonFirst = ButtonLeft,
    ButtonLast = 0x0000001F
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
    PointerEvent()
            : eventType(PointerMotion),
              x(0),
              y(0),
              wheelStep(0),
              button(ButtonLast),
              buttonMask(ButtonMaskNone),
              timestamp(direct_clock_get_millis())
    {
    }

    PointerEvent(PointerEventType type, int X, int Y, int step = 0,
                 PointerButton pbutton = ButtonLast, PointerButtonMask mask =
                         ButtonMaskNone)
            : eventType(type),
              x(X),
              y(Y),
              wheelStep(step),
              button(pbutton),
              buttonMask(mask),
              timestamp(direct_clock_get_millis())
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

class Widget;
struct PaintEvent
{
    enum PaintEventEye
    {
        LeftEye = 0x01, RightEye = 0x02, BothEyes = 0x03
    };

    PaintEvent()
    {
    }

    PaintEvent(Widget* widget, const PaintEvent& evt);

#ifdef ILIXI_STEREO_OUTPUT
    PaintEvent(Rectangle l, Rectangle r) :
    right(r), rect(l), eye(BothEyes)
    {
    }

    PaintEvent(Rectangle r, int disparity) :
    right(r), rect(r), eye(BothEyes)
    {
        right.translate(-disparity, 0);
        rect.translate(disparity, 0);
    }

    PaintEvent(Rectangle r, PaintEventEye e) :
    right(r), rect(r), eye(e)
    {
    }

    bool
    isValid()
    {
        if (eye & LeftEye)
        return rect.isValid();
        else
        return right.isValid();
    }

    Rectangle right;
    Rectangle rect;
#else
    PaintEvent(Rectangle r)
            : rect(r),
              eye(BothEyes)
    {
    }

    PaintEvent(Rectangle r, int disparity)
            : rect(r),
              eye(BothEyes)
    {
    }

    PaintEvent(Rectangle r, PaintEventEye e)
            : rect(r),
              eye(e)
    {
    }

    bool
    isValid()
    {
        return rect.isValid();
    }

    Rectangle rect;
#endif
    PaintEventEye eye;
};
}

#endif /* ILIXI_EVENTS_H_ */
