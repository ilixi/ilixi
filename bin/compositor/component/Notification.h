/*
 * Notification.h
 *
 *  Created on: Jul 24, 2012
 *      Author: tarik
 */

#ifndef NOTIFICATION_H_
#define NOTIFICATION_H_

#include <ui/SurfaceView.h>
#include <lib/Timer.h>
#include <lib/TweenAnimation.h>

namespace ilixi
{
class Compositor;

class Notification : public Widget
{
public:
    enum NotificationState
    {
        Init, Visible, Hidden
    };

    Notification(DFBSurfaceID id, Compositor* parent);

    virtual
    ~Notification();

    Size
    preferredSize() const;

    NotificationState
    state() const;

    void
    show(unsigned int ms);

protected:
    virtual void
    compose(const PaintEvent& event);

private:
    Compositor* _compositor;
    SurfaceView* _surface;
    NotificationState _state;
    Timer _timer;
    TweenAnimation _anim;
    Tween* _tween;

    void
    hide();

    void
    onNotificationGeomUpdate();

    void
    tweenSlot();

    void
    tweenEndSlot();
};

} /* namespace ilixi */
#endif /* NOTIFICATION_H_ */
