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

#ifndef NOTIFICATION_H_
#define NOTIFICATION_H_

#include <ui/SurfaceView.h>
#include <lib/Timer.h>
#include <lib/TweenAnimation.h>
#include <lib/Notify.h>
#include <lib/AnimationSequence.h>

namespace ilixi
{
class Compositor;

class Notification : public Widget
{
    friend class NotificationManager;
public:
    enum NotificationState
    {
        Init, Visible, Hidden
    };

    Notification(const Notify::NotifyData& data, Compositor* parent);

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
    NotificationState _notState;

    Image* _icon;
    std::string _title;
    std::string _text;
    std::string _sender;

    Timer _timer;

    AnimationSequence _seq;

    TweenAnimation* _animX;
    Tween* _tweenX;

    TweenAnimation* _animZ;
    Tween* _tweenZ;

    static Image* _bg;

    void
    hide();

    void
    onNotificationGeomUpdate();

    void
    tweenSlot();

    void
    tweenEndSlot();

    static void
    releaseBG();
};

} /* namespace ilixi */
#endif /* NOTIFICATION_H_ */
