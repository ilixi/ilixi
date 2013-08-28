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

#include <compositor/NotificationManager.h>
#include <compositor/Notification.h>
#include <compositor/Compositor.h>
#include <core/Logger.h>
#include <algorithm>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_NOTIFICATIONMAN, "ilixi/compositor/NotificationMan", "NotificationMan");

NotificationManager::NotificationManager(ILXCompositor* compositor)
        : _deltaY(0),
          _compositor(compositor),
          _maxNotifications(3)
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&_activeMutex, &attr);
    pthread_mutex_init(&_pendingMutex, NULL);

    _timer.sigExec.connect(sigc::mem_fun(this, &NotificationManager::updateNotifications));
    _timer.start(1500);

    _anim.setDuration(500);
    _tween = new Tween(Tween::SINE, Tween::EASE_OUT, 0, 1);
    _anim.addTween(_tween);
    _anim.sigExec.connect(sigc::mem_fun(this, &NotificationManager::tweenSlot));
}

NotificationManager::~NotificationManager()
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
    pthread_mutex_destroy(&_activeMutex);
    pthread_mutex_destroy(&_pendingMutex);
    Notification::releaseBG();
}

void
NotificationManager::addNotification(const Compositor::NotificationData& data)
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);

    // check if origin is allowed to push notifications.
    if (!checkPermission(data))
        return;

    // create a new notification.
    Notification* notification = new Notification(data, _compositor);
    Size s = notification->preferredSize();
    notification->setGeometry(_compositor->width() - 400, 10, 400, s.height());
    _compositor->addWidget(notification);
    notification->bringToFront();

    if (replacePending(notification) || replaceActive(notification))
        return;

    pthread_mutex_lock(&_pendingMutex);
    _pending.push_back(notification);
    pthread_mutex_unlock(&_pendingMutex);
//    arrangeNotifications(s.height());
}

void
NotificationManager::tweenSlot()
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);

    int deltaY = _tween->value() * _deltaY;
    ILOG_DEBUG(ILX_NOTIFICATIONMAN, "DeltaY: %d\n", deltaY);
    for (NotificationList::iterator it = _active.begin(); it != _active.end(); ++it)
    {
        if (*it == _active.back())
            continue;
        ((Notification*) *it)->translate(0, deltaY);
        ((Notification*) *it)->update();
    }

    _deltaY -= deltaY;
}

void
NotificationManager::arrangeNotifications(int deltaY)
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
    if (_active.empty())
        return;

    _anim.stop();
    _tween->setInitialValue(0);
    _tween->setEndValue(1);
    _deltaY += deltaY + 5;
    _anim.start();
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
}

void
NotificationManager::updateNotifications()
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);

    // remove hidden notifications, make space.
    pthread_mutex_lock(&_activeMutex);
    NotificationList::iterator it = _active.begin();
    while (it != _active.end())
    {
        if (((Notification*) *it)->state() == Notification::Hidden)
        {
            ILOG_DEBUG(ILX_NOTIFICATIONMAN, "Notification %p is removed.\n", ((Notification*) *it));
            _compositor->removeWidget(*it);
            it = _active.erase(it);
        } else
            ++it;
    }
    pthread_mutex_unlock(&_activeMutex);

    // if there is space add pending notifications.
    if (_active.size() < _maxNotifications)
    {
        bool arrange = false;
        pthread_mutex_lock(&_pendingMutex);
        unsigned int i;
        for (i = 0; i < _maxNotifications - _active.size(); ++i)
        {
            if (_pending.empty())
                break;
            _active.push_back(_pending.front());
            _pending.pop_front();
            _active.back()->show();
            arrange = true;
        }
        pthread_mutex_unlock(&_pendingMutex);

        if (arrange && _active.size() > 1)
            arrangeNotifications(i * _active.front()->preferredSize().height());
    }
}

bool
NotificationManager::replacePending(Notification* notification)
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
    if (notification->tag().empty())
        return false;
    pthread_mutex_lock(&_pendingMutex);
    NotificationList::iterator it = _pending.begin();
    while (it != _pending.end())
    {
        Notification* old = ((Notification*) *it);
        if (old->tag() == notification->tag())
        {
            it = _pending.erase(it);
            _pending.insert(it, notification);
            old->close();
            _compositor->removeWidget(old);
            pthread_mutex_unlock(&_pendingMutex);
            return true;
        }
        ++it;
    }
    pthread_mutex_unlock(&_pendingMutex);
    return false;
}

bool
NotificationManager::replaceActive(Notification* notification)
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);
    if (notification->tag().empty())
        return false;
    pthread_mutex_lock(&_activeMutex);
    NotificationList::iterator it = _active.begin();
    while (it != _active.end())
    {
        Notification* old = ((Notification*) *it);
        if (old->tag() == notification->tag())
        {
            it = _active.erase(it);
            _active.insert(it, notification);
            notification->setGeometry(old->surfaceGeometry());
            old->close();
            _compositor->removeWidget(old);
            notification->show();
            pthread_mutex_unlock(&_activeMutex);
            return true;
        }
        ++it;
    }
    pthread_mutex_unlock(&_activeMutex);
    return false;
}

void
NotificationManager::parseURI(const std::string& uri, std::string& query, std::string& path)
{
    typedef std::string::const_iterator iterator_t;

    iterator_t queryStart = std::find(uri.begin(), uri.end(), '?');
    iterator_t pathEnd = queryStart;

    if (queryStart != uri.end())
        queryStart++;

    query = std::string(queryStart, uri.end());

    iterator_t pathStart = uri.begin();

    for (size_t idx = 0; pathStart != pathEnd && idx < 3; idx++)
    {
        pathStart = std::find(pathStart, pathEnd, L'/');
        if (pathStart != pathEnd)
            pathStart++;
    }

    path = std::string(pathStart, pathEnd);
}

bool
NotificationManager::checkPermission(const Compositor::NotificationData& data)
{
    ILOG_TRACE_F(ILX_NOTIFICATIONMAN);

    // TODO check permission for notifications.
    if (data.client)
        return true;

    std::string query;
    std::string path;
    parseURI(data.origin, query, path);
    ILOG_DEBUG(ILX_NOTIFICATIONMAN, " -> path: %s\n", path.c_str());
    ILOG_DEBUG(ILX_NOTIFICATIONMAN, " -> query: %s\n", query.c_str());
    return true;
}

} /* namespace ilixi */
