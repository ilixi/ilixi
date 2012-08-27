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

Image* Notification::_bg = NULL;

Notification::Notification(const Notify::NotifyData& data, Compositor* parent)
        : Widget(parent),
          _compositor(parent),
          _notState(Init)
{
    ILOG_TRACE_W(ILX_NOTIFICATION);

    if (!_bg)
        _bg = new Image(ILIXI_DATADIR"compositor/notify.png");

    _title = data.title;
    _text = data.text;
    _sender = data.sender;
    if (data.path)
        _icon = new Image(data.path, 64, 64);
    else
        _icon = new Image(_compositor->appMan()->infoByName(_sender)->icon(),
                          64, 64);

    _timer.sigExec.connect(sigc::mem_fun(this, &Notification::hide));

    sigGeometryUpdated.connect(
            sigc::mem_fun(this, &Notification::onNotificationGeomUpdate));
    setVisible(false);

    _animZ = new TweenAnimation();
    _tweenZ = new Tween(Tween::SINE, Tween::EASE_OUT, 0, 1);
    _animZ->addTween(_tweenZ);
    _animZ->setDuration(400);
    _animZ->sigExec.connect(sigc::mem_fun(this, &Notification::tweenSlot));
    _seq.addAnimation(_animZ);

    _animX = new TweenAnimation();
    _tweenX = new Tween(Tween::SINE, Tween::EASE_OUT, 1, 0);
    _animX->addTween(_tweenX);
    _animX->setDuration(400);
    _animX->sigExec.connect(sigc::mem_fun(this, &Notification::tweenSlot));
    _animX->sigFinished.connect(
            sigc::mem_fun(this, &Notification::tweenEndSlot));
    _seq.addAnimation(_animX);
}

Notification::~Notification()
{
    delete _icon;
    ILOG_TRACE_W(ILX_NOTIFICATION);
}

Size
Notification::preferredSize() const
{
    return _bg->size();
}

Notification::NotificationState
Notification::state() const
{
    return _notState;
}

void
Notification::show(unsigned int ms)
{
    ILOG_TRACE_W(ILX_NOTIFICATION);
    if (!visible())
    {
        _seq.stop();
        _tweenX->setInitialValue(1);
        _tweenX->setEndValue(0);
        _tweenZ->setInitialValue(0);
        _tweenZ->setEndValue(1);
        _seq.start();
        _timer.start(5000, 1);
        setVisible(true);
    }
}

void
Notification::hide()
{
    ILOG_TRACE_W(ILX_NOTIFICATION);
    if (visible())
    {
        _seq.stop();
        _tweenX->setInitialValue(0);
        _tweenX->setEndValue(1);
        _tweenZ->setInitialValue(1);
        _tweenZ->setEndValue(0);
        _tweenZ->setEnabled(false);
        _seq.start();
    }
}

void
Notification::releaseBG()
{
    delete _bg;
}

void
Notification::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_NOTIFICATION);
    Painter p(this);
    p.begin(event);
    p.drawImage(_bg, width() * _tweenX->value(), 0);

    if (_tweenZ->enabled())
        p.stretchImage(
                _icon,
                Rectangle(20 + 32 * (1 - _tweenZ->value()),
                          5 + 32 * (1 - _tweenZ->value()),
                          64 * _tweenZ->value(), 64 * _tweenZ->value()));
    else
        p.drawImage(_icon, 20 + width() * _tweenX->value(), 5);

    p.setBrush(Color(255, 255, 255));
    p.setFont(*stylist()->defaultFont(StyleHint::TitleFont));
    p.drawText(_title, 90 + width() * _tweenX->value(), 15);
    p.setFont(*stylist()->defaultFont(StyleHint::DefaultFont));
    p.drawText(_text, 90 + width() * _tweenX->value(), 35);
    p.end();
}

void
Notification::onNotificationGeomUpdate()
{
}

void
Notification::tweenSlot()
{
//    setOpacity(1 - _tween->value());
    update();
}

void
Notification::tweenEndSlot()
{
    ILOG_TRACE_W(ILX_NOTIFICATION);
    if (_tweenX->value() == 1)
    {
        setVisible(false);
        _notState = Hidden;
    } else
        _notState = Visible;
}

} /* namespace ilixi */
