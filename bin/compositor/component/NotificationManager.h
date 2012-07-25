/*
 * NotificationManager.h
 *
 *  Created on: Jul 25, 2012
 *      Author: tarik
 */

#ifndef NOTIFICATIONMANAGER_H_
#define NOTIFICATIONMANAGER_H_

#include <vector>
#include <directfb.h>
#include <lib/Timer.h>
#include <lib/TweenAnimation.h>

namespace ilixi
{
class Compositor;
class Notification;

class NotificationManager
{
public:
    NotificationManager(Compositor* compositor);

    virtual
    ~NotificationManager();

    void
    addNotification(DFBSurfaceID id);

private:
    int _deltaY;
    Compositor* _compositor;
    typedef std::vector<Notification*> NotificationVector;
    NotificationVector _notifications;
    Timer _timer;
    TweenAnimation _anim;
    Tween* _tween;

    void
    removeNotifications();

    void
    arrangeNotifications(int deltaY);

    void
    tweenSlot();
};

} /* namespace ilixi */
#endif /* NOTIFICATIONMANAGER_H_ */
