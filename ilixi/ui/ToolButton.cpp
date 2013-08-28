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

#include <ui/ToolButton.h>
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

D_DEBUG_DOMAIN( ILX_TOOLBUTTONTH, "ilixi/ui/ToolButtonThread", "ToolButtonThread");
D_DEBUG_DOMAIN( ILX_TOOLBUTTON, "ilixi/ui/ToolButton", "ToolButton");

//*********************************************************************
// ToolButtonThread
//*********************************************************************
ToolButtonThread::ToolButtonThread()
        : Thread(),
          _target(NULL)
{
    ILOG_TRACE_F(ILX_TOOLBUTTONTH);
}

ToolButtonThread::~ToolButtonThread()
{
}

void
ToolButtonThread::setTarget(ToolButton* target)
{
    ILOG_TRACE_F(ILX_TOOLBUTTONTH);
    _target = target;
}

int
ToolButtonThread::run()
{
    ILOG_TRACE_F(ILX_TOOLBUTTONTH);
    int sleepNS = 300;
    if (!_target)
        return 1;

    while (true)
    {
        pthread_testcancel();
        if (!_target->enabled())
            return 1;
        _target->click(sleepNS);
        if (sleepNS > 50)
            sleepNS -= 50;
    }
    return 1;
}

//*********************************************************************
// ToolButton
//*********************************************************************
ToolButtonThread* ToolButton::__tbThread = NULL;

ToolButton::ToolButton(const std::string& text, Widget* parent)
        : Button(text, parent),
          _icon(NULL),
          _toolButtonStyle(IconBeforeText),
          _tbOptions(DrawFrame)
{
    if (__tbThread == NULL)
        __tbThread = new ToolButtonThread();

    ILOG_TRACE_W(ILX_TOOLBUTTON);
    setConstraints(FixedConstraint, FixedConstraint);
    _layout.setSingleLine(true);
    _layout.setAlignment(TextLayout::Center);
}

ToolButton::~ToolButton()
{
    ILOG_TRACE_W(ILX_TOOLBUTTON);
}

Size
ToolButton::preferredSize() const
{
    ILOG_TRACE_W(ILX_TOOLBUTTON);
    if (text().empty() && !icon())
        return stylist()->defaultSize(StyleHint::PushButton);

    int w = (_tbOptions & DrawFrame) ? stylist()->defaultParameter(StyleHint::ToolButtonLR) : 0;
    int h = (_tbOptions & DrawFrame) ? stylist()->defaultParameter(StyleHint::ToolButtonTB) : 0;

    if (checkable())
    {
        if ((_toolButtonStyle == IconBelowText) || (_toolButtonStyle == IconAboveText))
            h += stylist()->defaultParameter(StyleHint::ToolButtonIndicator) + stylist()->defaultParameter(StyleHint::ButtonOffset);
        else
            w += stylist()->defaultParameter(StyleHint::ToolButtonIndicator) + stylist()->defaultParameter(StyleHint::ButtonOffset);
    }

    if (_toolButtonStyle == TextOnly)
    {
        Size s = textExtents();
        if (s.isValid())
            return Size(w + s.width(), h + s.height());
        return stylist()->defaultSize(StyleHint::PushButton);
    }

    // calculate image size
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
        imgH = s.height() + 2; // 1px for button down movement
    }

    if (_toolButtonStyle == IconOnly)
    {
        ILOG_DEBUG(ILX_TOOLBUTTON, " -> icon only: %d, %d\n", w + imgW, h + imgH);
        if (imgW)
            return Size(w + imgW, h + imgH);
        return stylist()->defaultSize(StyleHint::PushButton);
    } else
    {
        Size s = textExtents();
        if (s.isValid() && imgW)
        {
            if (_toolButtonStyle == IconBeforeText)
            {
                w += imgW + stylist()->defaultParameter(StyleHint::ButtonOffset) + s.width();
                h += std::max(s.height(), imgH);
                return Size(w, h);
            } else
            {
                // IconBelowText or IconAboveText
                w += std::max(imgW, s.width());
                h += imgH //+ s.height()
                + stylist()->defaultParameter(StyleHint::ButtonOffset);
                return Size(w, h);
            }
        } else if (imgW)
            return Size(w + imgW, h + imgH);
        else
            return Size(w + s.width(), h + s.height());
    }
}

Icon*
ToolButton::icon() const
{
    return _icon;
}

bool
ToolButton::drawFrame() const
{
    return (_tbOptions & DrawFrame);
}

ToolButton::ToolButtonStyle
ToolButton::toolButtonStyle() const
{
    return _toolButtonStyle;
}

void
ToolButton::setToolButtonStyle(ToolButtonStyle style)
{
    _toolButtonStyle = style;
}

void
ToolButton::setIcon(Icon* icon)
{
    ILOG_TRACE_W(ILX_TOOLBUTTON);
    removeChild(_icon);
    _icon = icon;
    addChild(_icon);
}

void
ToolButton::setIcon(const std::string& iconPath, const Size& size)
{
    ILOG_TRACE_W(ILX_TOOLBUTTON);
    removeChild(_icon);
    _icon = new Icon(iconPath, this);
    setIconSize(size);
    addChild(_icon);
}

void
ToolButton::setIcon(StyleHint::PackedIcon icon, const Size& size)
{
    ILOG_TRACE_W(ILX_TOOLBUTTON);
    removeChild(_icon);
    _icon = new Icon(icon, this);
    if (size.isValid())
        _icon->setSize(size);
    addChild(_icon);
}

void
ToolButton::setIconSize(const Size& size)
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
ToolButton::setDrawFrame(bool drawFrame)
{
    if (drawFrame)
        _tbOptions = (ToolButtonOptions) (_tbOptions | DrawFrame);
    else
        _tbOptions = (ToolButtonOptions) (_tbOptions & ~DrawFrame);
}

void
ToolButton::setRepeatable(bool repeatable)
{
    ILOG_TRACE_W(ILX_TOOLBUTTON);
    if (repeatable)
        _tbOptions = (ToolButtonOptions) (_tbOptions | Repeatable);
    else
        _tbOptions = (ToolButtonOptions) (_tbOptions & ~Repeatable);
}

void
ToolButton::pointerButtonDownEvent(const PointerEvent& event)
{
    ILOG_TRACE_W(ILX_TOOLBUTTON);
    if (_tbOptions & Repeatable)
    {
        __tbThread->cancel();
        __tbThread->setTarget(this);
        __tbThread->start();
    }

    _buttonFlag = (ButtonFlags) (_buttonFlag | PressedDown);
    if (_icon)
        _icon->setState(PressedState);

    update();
    sigPressed();
}

void
ToolButton::pointerButtonUpEvent(const PointerEvent& event)
{
    ILOG_TRACE_W(ILX_TOOLBUTTON);
    sigReleased();
    if (_buttonFlag & PressedDown)
    {
        if (__tbThread->_target == this && (_tbOptions & Repeatable))
            __tbThread->cancel();

        _buttonFlag = (ButtonFlags) (_buttonFlag & ~PressedDown);
        if (_icon)
            _icon->setState(DefaultState);
        PlatformManager::instance().playSoundEffect("Click");
        toggleChecked();
        sigClicked();
    }
}

void
ToolButton::leaveEvent(const PointerEvent& event)
{
    ILOG_TRACE_W(ILX_TOOLBUTTON);
    if (__tbThread->_target == this && (_tbOptions & Repeatable))
        __tbThread->cancel();

    update();
}

void
ToolButton::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_TOOLBUTTON);
    Painter p(this);
    p.begin(event);
    stylist()->drawToolButton(&p, this);
    p.end();
}

void
ToolButton::updateTextBaseGeometry()
{
    ILOG_TRACE_W(ILX_TOOLBUTTON);
    int textHeight = textExtents().height();
    int iconW = 0;
    int iconH = 0;
    int wUsed = drawFrame() ? stylist()->defaultParameter(StyleHint::ToolButtonLR) : 0;
    int x = drawFrame() ? stylist()->defaultParameter(StyleHint::ToolButtonLeft) : 0;

    if (checkable())
    {
        if ((_toolButtonStyle == TextOnly) || (_toolButtonStyle == IconOnly) || (_toolButtonStyle == IconBeforeText))
        {
            x += stylist()->defaultParameter(StyleHint::ToolButtonIndicator) + stylist()->defaultParameter(StyleHint::ButtonOffset);
            wUsed += stylist()->defaultParameter(StyleHint::ToolButtonIndicator) + stylist()->defaultParameter(StyleHint::ButtonOffset);
        }
    }

    if (_icon)
    {
        if (!_icon->size().isValid())
            _icon->setSize(_icon->preferredSize());
        iconW = _icon->width();
        iconH = _icon->height() + 1;
    }

    if (_toolButtonStyle == TextOnly)
    {
        int y = (height() - textHeight) / 2;
        _layout.setBounds(x, y, width() - wUsed, textHeight);
    }

    else if (_toolButtonStyle == IconOnly)
    {
        _icon->moveTo((width() - iconW) / 2, (height() - iconH) / 2);
        _layout.setBounds(0, 0, 0, 0);
        return;
    }

    else if (_toolButtonStyle == IconBeforeText)
    {
        if (iconW)
        {
            _icon->moveTo(x, (height() - iconH) / 2);
            x += iconW + stylist()->defaultParameter(StyleHint::ButtonOffset);
            wUsed += iconW + stylist()->defaultParameter(StyleHint::ButtonOffset);
        }

        _layout.setBounds(x, (height() - textHeight) / 2, width() - wUsed, textHeight);

    } else if (_toolButtonStyle == IconBelowText)
    {
        _layout.setBounds(x, stylist()->defaultParameter(StyleHint::ToolButtonTop), width() - wUsed, textHeight);
        if (iconW)
            _icon->moveTo((width() - iconW) / 2, stylist()->defaultParameter(StyleHint::ToolButtonTop) + textHeight + 1 + stylist()->defaultParameter(StyleHint::ButtonOffset));
    } else //  IconAboveText
    {
        int y = stylist()->defaultParameter(StyleHint::ToolButtonTop);

        if (iconW)
        {
            _icon->moveTo((width() - iconW) / 2, y + 1);
            y += iconH + stylist()->defaultParameter(StyleHint::ButtonOffset) + 1;
        }

        _layout.setBounds(x, y, width() - wUsed, textHeight);
    }
    _layout.doLayout(font());
}

} /* namespace ilixi */
