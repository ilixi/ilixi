/*
 * Notification.cpp
 *
 *  Created on: Jul 24, 2012
 *      Author: tarik
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
        : Widget(parent), _compositor(parent), _surface(NULL), _state(Init)
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
    return _surface->preferredSize();
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
        _timer.start(2000, 1);
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
//    ILOG_TRACE_W(ILX_NOTIFICATION);
//    Painter painter(this);
//    painter.begin(event);
//    painter.setBrush(Color(255, 0, 0, 50));
//    painter.fillRectangle(0, 0, width(), height());
//    painter.end();
}

void
Notification::onNotificationGeomUpdate()
{
    _surface->setGeometry(0, 0, width(), height());
}

void
Notification::tweenSlot()
{
    _surface->setX(_tween->value() * width());
    update();
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
