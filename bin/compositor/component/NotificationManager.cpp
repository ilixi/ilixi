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
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
    _timer.sigExec.connect(
            sigc::mem_fun(this, &NotificationManager::removeNotifications));
    _timer.start(10000);

    _anim.setDuration(500);
    _tween = new Tween(Tween::SINE, Tween::EASE_OUT, 0, 1);
    _anim.addTween(_tween);
    _anim.sigExec.connect(sigc::mem_fun(this, &NotificationManager::tweenSlot));

    pthread_mutex_init(&_notMutex, NULL);
}

NotificationManager::~NotificationManager()
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
    Notification::releaseBG();
    pthread_mutex_destroy(&_notMutex);
}

void
NotificationManager::addNotification(const Notify::NotifyData& data)
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
    Notification* notify = new Notification(data, _compositor);
    pthread_mutex_lock(&_notMutex);
    _notifications.push_back(notify);
    Size s = notify->preferredSize();
    notify->setGeometry(_compositor->width() - s.width(), 10, s.width(),
                        s.height());

    _compositor->addWidget(notify);
    notify->bringToFront();
    arrangeNotifications(s.height());
    notify->show(500);
    pthread_mutex_unlock(&_notMutex);
}

void
NotificationManager::removeNotifications()
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
    pthread_mutex_lock(&_notMutex);
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
    pthread_mutex_unlock(&_notMutex);
}

void
NotificationManager::arrangeNotifications(int deltaY)
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
    if (_notifications.empty())
        return;

    _anim.stop();
    _tween->setInitialValue(0);
    _tween->setEndValue(1);
    _deltaY += deltaY + 5;
    _anim.start();
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
}

void
NotificationManager::tweenSlot()
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
    pthread_mutex_lock(&_notMutex);
    int deltaY = _tween->value() * _deltaY;
    ILOG_DEBUG(ILX_NOTIFICATIONMAN, "DeltaY: %d\n", deltaY);
    for (unsigned int i = 0; i < _notifications.size() - 1; ++i)
    {
        _notifications[i]->translate(0, deltaY);
        _notifications[i]->update();
    }

    _deltaY -= deltaY;
    pthread_mutex_unlock(&_notMutex);
}

} /* namespace ilixi */
