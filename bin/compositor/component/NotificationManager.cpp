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

#include "NotificationManager.h"
#include "Notification.h"
#include "Compositor.h"
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_NOTIFICATIONMAN, "ilixi/comp/NotificationMan",
               "NotificationMan");

NotificationManager::NotificationManager(Compositor* compositor)
        : _deltaY(0),
          _compositor(compositor)
{
    _timer.sigExec.connect(
            sigc::mem_fun(this, &NotificationManager::removeNotifications));
    _timer.start(1000);

    _anim.setDuration(100);
    _tween = new Tween(Tween::BOUNCE, Tween::EASE_OUT, 0, 1);
    _anim.addTween(_tween);
    _anim.sigExec.connect(sigc::mem_fun(this, &NotificationManager::tweenSlot));

}

NotificationManager::~NotificationManager()
{
}

void
NotificationManager::addNotification(DFBSurfaceID id)
{
    Notification* notify = new Notification(id, _compositor);
    _notifications.push_back(notify);
    Size s = notify->preferredSize();
    notify->setGeometry(_compositor->width() - s.width(),
                        _compositor->height() - s.height() - 10, s.width(),
                        s.height());

    notify->_xStart = _compositor->width() - s.width();

    _compositor->addWidget(notify);
    _compositor->widgetToFront(notify);
    arrangeNotifications(s.height());
    notify->show(500);
}

void
NotificationManager::removeNotifications()
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
    NotificationVector::iterator it = _notifications.begin();
    while (it != _notifications.end())
    {
        if (((Notification*) *it)->state() == Notification::Hidden)
        {
            ILOG_DEBUG(ILX_NOTIFICATIONMAN,
                       "Notification %p is removed.\n", ((Notification*) *it));
            _compositor->removeWidget(*it);
            it = _notifications.erase(it);
        } else
            ++it;
    }
}

void
NotificationManager::arrangeNotifications(int deltaY)
{
    if (_notifications.empty())
        return;

    _anim.stop();
    _deltaY += deltaY + 5;
    _anim.start();
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
}

void
NotificationManager::tweenSlot()
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
    int deltaY = _tween->value() * _deltaY;
    for (unsigned int i = 0; i < _notifications.size() - 1; ++i)
    {
        _notifications[i]->translate(0, -deltaY);
        _notifications[i]->update();
    }

    _deltaY -= deltaY;
}

} /* namespace ilixi */
