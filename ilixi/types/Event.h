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
/*!
 * Maps from DFBWindowEventType.
 */
enum KeyEventType
{
    KeyDownEvent = DWET_KEYDOWN,
    KeyUpEvent = DWET_KEYUP
};

/*!
 * Defines key input events.
 */
struct KeyEvent
{
    KeyEvent(KeyEventType type, DFBInputDeviceKeySymbol symbol)
            : eventType(type),
              keySymbol(symbol),
              keyID(DIKI_UNKNOWN),
              modifierMask(DFBInputDeviceModifierMask(0)),
              lockState(DFBInputDeviceLockState(0))
    {
    }

    KeyEvent(KeyEventType type, DFBWindowEvent event)
            : eventType(type),
              keySymbol(event.key_symbol),
              keyID(event.key_id),
              modifierMask(event.modifiers),
              lockState(event.locks)
    {
    }

    KeyEvent(KeyEventType type, DFBInputDeviceKeySymbol symbol, DFBInputDeviceKeyIdentifier id, DFBInputDeviceModifierMask mask, DFBInputDeviceLockState locks)
            : eventType(type),
              keySymbol(symbol),
              keyID(id),
              modifierMask(mask),
              lockState(locks)
    {
    }

    //! Type of event.
    KeyEventType eventType;
    //! Advanced mapping unicode compatible and modifier dependent.
    DFBInputDeviceKeySymbol keySymbol;
    //! Basic mapping and modifier independent.
    DFBInputDeviceKeyIdentifier keyID;
    //! Pressed modifiers.
    DFBInputDeviceModifierMask modifierMask;
    //! Active locks.
    DFBInputDeviceLockState lockState;
};

/*!
 * Maps from DFBWindowEventType.
 */
enum PointerEventType
{
    PointerButtonDown = DWET_BUTTONDOWN,
    PointerButtonUp = DWET_BUTTONUP,
    PointerMotion = DWET_MOTION,
    PointerWheel = DWET_WHEEL
};

/*!
 * Maps from DFBInputDeviceButtonIdentifier.
 */
enum PointerButton
{
    ButtonLeft = DIBI_LEFT,
    ButtonRight = DIBI_RIGHT,
    ButtonMiddle = DIBI_MIDDLE,
    ButtonFirst = DIBI_FIRST,
    ButtonLast = DIBI_LAST
};

/*!
 * Maps from DFBInputDeviceButtonMask.
 */
enum PointerButtonMask
{
    ButtonMaskNone = 0,
    ButtonMaskLeft = DIBM_LEFT,
    ButtonMaskRight = DIBM_RIGHT,
    ButtonMaskMiddle = DIBM_MIDDLE
};

/*!
 * Defines pointer input events.
 */
struct PointerEvent
{
    PointerEvent()
            : eventType(PointerMotion),
              x(0),
              y(0),
              wheelStep(0),
              button(ButtonLast),
              buttonMask(ButtonMaskNone),
              modifierMask((DFBInputDeviceModifierMask) 0),
              timestamp(direct_clock_get_millis())
    {
    }

    PointerEvent(PointerEventType type, DFBWindowEvent event)
            : eventType(type),
              x(event.x),
              y(event.y),
              wheelStep(event.step),
              button((PointerButton) event.button),
              buttonMask((PointerButtonMask) event.buttons),
              modifierMask(event.modifiers),
              timestamp(direct_clock_get_millis())
    {
    }

    PointerEvent(PointerEventType type, int X, int Y, int step = 0, PointerButton pbutton = ButtonLast, PointerButtonMask mask = ButtonMaskNone)
            : eventType(type),
              x(X),
              y(Y),
              wheelStep(step),
              button(pbutton),
              buttonMask(mask),
              modifierMask((DFBInputDeviceModifierMask) 0),
              timestamp(direct_clock_get_millis())
    {
    }

    PointerEvent(const PointerEvent& other)
            : eventType(other.eventType),
              x(other.x),
              y(other.y),
              wheelStep(other.wheelStep),
              button(other.button),
              buttonMask(other.buttonMask),
              modifierMask(other.modifierMask),
              timestamp(other.timestamp)
    {
    }

    //! Type of event.
    PointerEventType eventType;
    //! Cursor X position.
    int x;
    //! Cursor Y position.
    int y;
    //! Wheel step {-1, 1}.
    int wheelStep;
    //! Button being pressed or released.
    PointerButton button;
    //! Mask of currently pressed buttons.
    PointerButtonMask buttonMask;
    //! Pressed modifiers.
    DFBInputDeviceModifierMask modifierMask;
    //! Set on construction.
    long timestamp;
};

class Widget;
struct PaintEvent
{
    enum PaintEventEye
    {
        LeftEye = 0x01,
        RightEye = 0x02,
        BothEyes = 0x03
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

struct UniversalEvent
{
    UniversalEvent(Widget* Target, unsigned int Type, void* Data)
    {
        universal.clazz = DFEC_UNIVERSAL;
        universal.size = sizeof(UniversalEvent);
        target = Target;
        type = Type;
        data = Data;
    }

    UniversalEvent(const UniversalEvent& other) :
        universal(other.universal),
        target(other.target),
        type(other.type),
        data(other.data)
    {
    }

    DFBUniversalEvent universal;
    Widget* target;
    unsigned int type;
    void* data;
};

} // namespace ilixi

#endif /* ILIXI_EVENTS_H_ */
