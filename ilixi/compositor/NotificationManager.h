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

#ifndef ILIXI_NOTIFICATIONMANAGER_H_
#define ILIXI_NOTIFICATIONMANAGER_H_

#include <directfb.h>
#include <lib/Timer.h>
#include <lib/TweenAnimation.h>
#include <lib/Notify.h>
#include <core/ComponentData.h>
#include <list>

namespace ilixi
{
class ILXCompositor;
class Notification;

//! Manages and displays incoming notifications.
class NotificationManager
{
public:
    /*!
     * Constructor.
     */
    NotificationManager(ILXCompositor* compositor);

    /*!
     * Destructor.
     */
    virtual
    ~NotificationManager();

    /*!
     * Adds a new notification to queue.
     */
    void
    addNotification(const Compositor::NotificationData& data);

private:
    int _deltaY;
    ILXCompositor* _compositor;
    unsigned int _maxNotifications;
    typedef std::list<Notification*> NotificationList;

    NotificationList _active;
    pthread_mutex_t _activeMutex;
    NotificationList _pending;
    pthread_mutex_t _pendingMutex;

    Timer _timer;
    TweenAnimation _anim;
    Tween* _tween;

    void
    tweenSlot();

    void
    updateNotifications();

    void
    arrangeNotifications(int deltaY);

    /*!
     * Returns true if replaces an old notification in pending queue.
     *
     * Replaced notification will fire a CLOSE event.
     */
    bool
    replacePending(Notification* notification);

    /*!
     * Returns true if replaces an old (visible) notification in active queue.
     *
     * Replaced notification will fire a CLOSE event.
     * New notification will fire a SHOW event.
     */
    bool
    replaceActive(Notification* notification);

    void
    parseURI(const std::string& uri, std::string& query, std::string& path);

    /*!
     * Returns true if notification origin is allowed to show notifications.
     */
    bool
    checkPermission(const Compositor::NotificationData& data);
};

} /* namespace ilixi */
#endif /* ILIXI_NOTIFICATIONMANAGER_H_ */
