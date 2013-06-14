/*
 Copyright 2010-2013 Tarik Sekmen.

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

#include <ui/DirectionalButton.h>
#include <core/PlatformManager.h>
#include <graphics/Painter.h>
#include <core/Logger.h>

#ifdef ANDROID_NDK
extern "C" {
#include <compat/pthread_cancel.h>
}
#endif

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_DIRBUTTON, "ilixi/ui/DirectionalButton", "DirectionalButton");
D_DEBUG_DOMAIN( ILX_DIRBUTTONTH, "ilixi/ui/DirectionalButtonThread", "DirectionalButtonThread");

//*********************************************************************
// DirectionalButtonThread
//*********************************************************************
DirectionalButtonThread::DirectionalButtonThread()
        : Thread(),
          _target(NULL)
{
    ILOG_TRACE_F(ILX_DIRBUTTONTH);
}

DirectionalButtonThread::~DirectionalButtonThread()
{
}

void
DirectionalButtonThread::setTarget(DirectionalButton* target)
{
    ILOG_TRACE_F(ILX_DIRBUTTONTH);
    _target = target;
}

int
DirectionalButtonThread::run()
{
    ILOG_TRACE_F(ILX_DIRBUTTONTH);
    int sleepNS = 300;
    if (!_target)
        return 1;

    while (true)
    {
        pthread_testcancel();
        if (!_target->enabled())
            return 1;
        _target->click(sleepNS);
        if (sleepNS > 75)
            sleepNS -= 25;
    }
    return 1;
}

//*********************************************************************
// DirectionalButton
//*********************************************************************
DirectionalButtonThread* DirectionalButton::__dbThread = NULL;

DirectionalButton::DirectionalButton(const std::string& text, Widget* parent)
        : Button(text, parent),
          _corners(TopCorners),
          _buttonType(Arrow),
          _buttonDirection(Up),
          _icon(NULL)
{
    ILOG_TRACE_W(ILX_DIRBUTTON);
    if (__dbThread == NULL)
        __dbThread = new DirectionalButtonThread();

    setConstraints(FixedConstraint, FixedConstraint);
    _layout.setSingleLine(true);
    _layout.setAlignment(TextLayout::Center);
}

DirectionalButton::~DirectionalButton()
{
    ILOG_TRACE_W(ILX_DIRBUTTON);
}

Size
DirectionalButton::preferredSize() const
{
    ILOG_TRACE_W(ILX_DIRBUTTON);

    int imgW = 0;
    int imgH = 0;
    if (_icon)
    {
        Size s;
        if (_icon->size().isValid())
            s = _icon->size();
        else
            s = _icon->preferredSize();
        imgW = s.width();
        imgH = s.height();
    }

    Size s = textExtents();
    int w = 0;
    if (!text().empty())
    {
        if (_icon)
            w = stylist()->defaultParameter(StyleHint::ButtonOffset);
        w += s.width();
    }

    return Size(w + imgW + stylist()->defaultParameter(StyleHint::ToolButtonLR), std::max(s.height(), imgH) + stylist()->defaultParameter(StyleHint::ToolButtonTB));
}

Direction
DirectionalButton::buttonDirection() const
{
    return _buttonDirection;
}

DirectionalButton::DirectionalButtonType
DirectionalButton::buttonType() const
{
    return _buttonType;
}

Corners
DirectionalButton::corners() const
{
    return _corners;
}

void
DirectionalButton::setButtonDirection(Direction buttonDirection)
{
    _buttonDirection = buttonDirection;
    if (_buttonType != Custom)
    {
        if (_buttonDirection == Up)
            _corners = TopCorners;
        else if (_buttonDirection == Down)
            _corners = BottomCorners;
        else if (_buttonDirection == Left)
            _corners = LeftCorners;
        else
            _corners = RightCorners;
    }
}

void
DirectionalButton::setButtonType(DirectionalButtonType buttonType)
{
    _buttonType = buttonType;
}

void
DirectionalButton::setCorners(Corners corners)
{
    _corners = corners;
}

void
DirectionalButton::setIcon(const std::string& iconPath, const Size& size)
{
    ILOG_TRACE_W(ILX_DIRBUTTON);
    removeChild(_icon);
    _icon = new Icon(iconPath, this);
    setIconSize(size);
    addChild(_icon);
}

void
DirectionalButton::setIcon(StyleHint::PackedIcon icon, const Size& size)
{
    ILOG_TRACE_W(ILX_DIRBUTTON);
    removeChild(_icon);
    _icon = new Icon(icon, this);
    if (size.isValid())
        _icon->setSize(size);
    addChild(_icon);
}

void
DirectionalButton::setIconSize(const Size& size)
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
DirectionalButton::pointerButtonDownEvent(const PointerEvent& event)
{
    ILOG_TRACE_W(ILX_DIRBUTTON);

    __dbThread->cancel();
    __dbThread->setTarget(this);
    __dbThread->start();

    _buttonFlag = (ButtonFlags) (_buttonFlag | PressedDown);
    if (_icon)
        _icon->setState(PressedState);

    update();
    sigPressed();
}

void
DirectionalButton::pointerButtonUpEvent(const PointerEvent& event)
{
    ILOG_TRACE_W(ILX_DIRBUTTON);
    sigReleased();
    if (_buttonFlag & PressedDown)
    {
        if (__dbThread->_target == this)
            __dbThread->cancel();

        _buttonFlag = (ButtonFlags) (_buttonFlag & ~PressedDown);
        if (_icon)
            _icon->setState(DefaultState);
        PlatformManager::instance().playSoundEffect("Click");
        toggleChecked();
        sigClicked();
    }
}

void
DirectionalButton::leaveEvent(const PointerEvent& event)
{
    ILOG_TRACE_W(ILX_DIRBUTTON);
    if (__dbThread->_target == this)
        __dbThread->cancel();

    update();
}

void
DirectionalButton::compose(const ilixi::PaintEvent& event)
{
    ILOG_TRACE_W(ILX_DIRBUTTON);
    Painter p(this);
    p.begin(event);
    stylist()->drawDirectionalButton(&p, this);
}

void
DirectionalButton::updateTextBaseGeometry()
{
    ILOG_TRACE_W(ILX_DIRBUTTON);
    int textHeight = textExtents().height();
    int iconW = 0;
    int iconH = 0;
    int wUsed = stylist()->defaultParameter(StyleHint::ToolButtonLR);
    int x = stylist()->defaultParameter(StyleHint::ToolButtonLeft);

    if (_icon)
    {
        if (!_icon->size().isValid())
            _icon->setSize(_icon->preferredSize());
        iconW = _icon->width();
        iconH = _icon->height();

        _icon->moveTo(x, (height() - iconH) / 2);
        x += iconW + stylist()->defaultParameter(StyleHint::ButtonOffset);
        wUsed += iconW + stylist()->defaultParameter(StyleHint::ButtonOffset);
    }

    _layout.setBounds(x, (height() - textHeight) / 2, width() - wUsed, textHeight);
    _layout.doLayout(font());
}

} /* namespace ilixi */
