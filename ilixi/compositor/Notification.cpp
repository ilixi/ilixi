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

#include <compositor/Notification.h>
#include <compositor/Compositor.h>
#include <graphics/Painter.h>
#include <core/PlatformManager.h>
#include <core/Logger.h>
#include <sigc++/signal.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_NOTIFICATION, "ilixi/compositor/Notification", "Notification");

Image* Notification::_bg = NULL;

Notification::Notification(const Compositor::NotificationData& data, ILXCompositor* parent)
        : Widget(parent->appWindow()),
          _compositor(parent),
          _notState(Init),
          _clicked(false)
{
    ILOG_TRACE_W(ILX_NOTIFICATION);
    setInputMethod(PointerInput);

    if (!_bg)
        _bg = new Image(ILIXI_DATADIR"images/notify.png");

    _client = data.client;
    _tag = data.tag;
    _text = data.body;
    _title = data.title;
    snprintf(_uuid, 37, "%s", data.uuid);
    if (strcmp(data.icon, "") != 0)
        _icon = new Image(data.icon, 64, 64);
    else
    {
        AppInfo* info = _compositor->appMan()->infoByPID(_client);
        if (info)
            _icon = new Image(info->icon(), 64, 64);
        else
            _icon = stylist()->defaultIcon(StyleHint::Cross);
    }
    _timer.sigExec.connect(sigc::mem_fun(this, &Notification::hide));

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
    _animX->sigFinished.connect(sigc::mem_fun(this, &Notification::tweenEndSlot));
    _seq.addAnimation(_animX);

    setVisible(false);
    ILOG_DEBUG(ILX_NOTIFICATION, " -> body: %s\n", data.body);
    ILOG_DEBUG(ILX_NOTIFICATION, " -> client: %d\n", _client);
    ILOG_DEBUG(ILX_NOTIFICATION, " -> iconURL: %s\n", data.icon);
    ILOG_DEBUG(ILX_NOTIFICATION, " -> title: %s\n", data.title);
    ILOG_DEBUG(ILX_NOTIFICATION, " -> uuid: %s\n", data.uuid);
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

const std::string&
Notification::tag() const
{
    return _tag;
}

const std::string&
Notification::text() const
{
    return _text;
}

const std::string&
Notification::title() const
{
    return _title;
}

Notification::NotificationState
Notification::state() const
{
    return _notState;
}

void
Notification::show()
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
        _timer.start(_compositor->settings.notificationTimeout, 1);
        PlatformManager::instance().playSoundEffect("Notification");
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
Notification::close()
{
    _compositor->_compComp->signalNotificationAck(Compositor::Close, _uuid, _client);
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
    p.stretchImage(_bg, width() * _tweenX->value(), 0, width(), height());

    if (_tweenZ->enabled())
        p.stretchImage(_icon, Rectangle(20 + 32 * (1 - _tweenZ->value()), 5 + 32 * (1 - _tweenZ->value()), 64 * _tweenZ->value(), 64 * _tweenZ->value()));
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
Notification::pointerButtonDownEvent(const PointerEvent& event)
{
    _clicked = true;
}

void
Notification::pointerButtonUpEvent(const PointerEvent& event)
{
    if (_clicked)
    {
        _compositor->_compComp->signalNotificationAck(Compositor::Click, _uuid, _client);
        _clicked = false;
    }
}

void
Notification::tweenSlot()
{
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
        close();
    } else
    {
        _notState = Visible;
        _compositor->_compComp->signalNotificationAck(Compositor::Show, _uuid, _client);
    }
}

} /* namespace ilixi */
