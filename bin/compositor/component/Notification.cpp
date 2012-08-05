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

#include "Notification.h"
#include "Compositor.h"
#include <graphics/Painter.h>
#include <core/Logger.h>
#include <sigc++/signal.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_NOTIFICATION, "ilixi/comp/Notification", "Notification");

Notification::Notification(DFBSurfaceID sid, Compositor* parent)
        : Widget(parent),
          _compositor(parent),
          _surface(NULL),
          _state(Init)
{
    ILOG_TRACE_W(ILX_NOTIFICATION);
    _surface = new SurfaceView();
    _surface->setSourceFromSurfaceID(sid);
    addChild(_surface);

    _timer.sigExec.connect(sigc::mem_fun(this, &Notification::hide));

    sigGeometryUpdated.connect(
            sigc::mem_fun(this, &Notification::onNotificationGeomUpdate));
    setVisible(false);

    _tween = new Tween(Tween::SINE, Tween::EASE_OUT, 1, 0);
    _anim.addTween(_tween);
    _anim.sigExec.connect(sigc::mem_fun(this, &Notification::tweenSlot));
    _anim.sigFinished.connect(sigc::mem_fun(this, &Notification::tweenEndSlot));
}

Notification::~Notification()
{
    ILOG_TRACE_W(ILX_NOTIFICATION);
}

Size
Notification::preferredSize() const
{
    Size s = _surface->preferredSize();
    return Size(
            s.width() + stylist()->defaultParameter(StyleHint::FrameOffsetLR),
            s.height() + stylist()->defaultParameter(StyleHint::FrameOffsetTB));
}

Notification::NotificationState
Notification::state() const
{
    return _state;
}

void
Notification::show(unsigned int ms)
{
    ILOG_TRACE_W(ILX_NOTIFICATION);
    if (!visible())
    {
        _anim.setDuration(ms);
        _anim.stop();
        _tween->setInitialValue(1);
        _tween->setEndValue(0);
        _anim.start();
        _timer.start(3000, 1);
        _state = Visible;
        setVisible(true);
    }
}

void
Notification::hide()
{
    ILOG_TRACE_W(ILX_NOTIFICATION);
    if (visible())
    {
        _anim.stop();
        _tween->setInitialValue(0);
        _tween->setEndValue(1);
        _anim.start();
    }
}

void
Notification::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_NOTIFICATION);
    Painter painter(this);
    painter.begin(event);
    stylist()->drawFrame(&painter, 0, 0, width(), height(), LeftCorners);
    painter.end();
}

void
Notification::onNotificationGeomUpdate()
{
    _surface->setGeometry(
            stylist()->defaultParameter(StyleHint::FrameOffsetLeft),
            stylist()->defaultParameter(StyleHint::FrameOffsetTop),
            width() - stylist()->defaultParameter(StyleHint::FrameOffsetLR),
            height() - stylist()->defaultParameter(StyleHint::FrameOffsetTB));
}

void
Notification::tweenSlot()
{
//    setOpacity(1 - _tween->value());
//    update();
}

void
Notification::tweenEndSlot()
{
    ILOG_TRACE_W(ILX_NOTIFICATION);
    if (_tween->value() == 1)
    {
        setVisible(false);
        _state = Hidden;
    }
}

} /* namespace ilixi */
