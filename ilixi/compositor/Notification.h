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

#ifndef ILIXI_NOTIFICATION_H_
#define ILIXI_NOTIFICATION_H_

#include <ui/SurfaceView.h>
#include <lib/Timer.h>
#include <lib/TweenAnimation.h>
#include <core/ComponentData.h>
#include <lib/AnimationSequence.h>

namespace ilixi
{
class ILXCompositor;

//! Used for displaying a simple desktop notification.
class Notification : public Widget
{
    friend class NotificationManager;
public:
    /*!
     * This enum specifies states for a notification.
     */
    enum NotificationState
    {
        Init,       //!< Notification is initialized.
        Visible,    //!< Notification is visible.
        Hidden      //!< Notification is hidden.
    };

    /*!
     * Constructor creates a new notification.
     *
     * @param data for notification.
     * @param parent widget.
     */
    Notification(const Compositor::NotificationData& data, ILXCompositor* parent);

    /*!
     * Destructor.
     */
    virtual
    ~Notification();

    /*!
     * Returns fixed notification size.
     */
    Size
    preferredSize() const;

    /*!
     * Returns tag of notification.
     */
    const std::string&
    tag() const;

    /*!
     * Returns body of notification.
     */
    const std::string&
    text() const;

    /*!
     * Returns title of notification.
     */
    const std::string&
    title() const;

    /*!
     * Returns notification state.
     */
    NotificationState
    state() const;

    /*!
     * Shows notification.
     */
    void
    show();

    /*!
     * Hides and closes notification.
     */
    void
    hide();

    /*!
     * Sends a close event via Compositor component.
     */
    void
    close();

protected:
    /*!
     * Draws notification.
     */
    virtual void
    compose(const PaintEvent& event);

    /*!
     * Implements click.
     */
    virtual void
    pointerButtonDownEvent(const PointerEvent& event);

    /*!
     * Emits sigClicked if clicked.
     */
    virtual void
    pointerButtonUpEvent(const PointerEvent& event);

private:
    ILXCompositor* _compositor;
    NotificationState _notState;

    bool _clicked;

    Image* _icon;
    std::string _title;
    std::string _text;
    std::string _tag;
    char _uuid[37];
    pid_t _client;

    //! This timer is used for hiding notification.
    Timer _timer;

    AnimationSequence _seq;
    TweenAnimation* _animX;
    Tween* _tweenX;
    TweenAnimation* _animZ;
    Tween* _tweenZ;

    //! Background image used by all notifications.
    static Image* _bg;

    //! Just updates widget.
    void
    tweenSlot();

    //! Emits signal when animation is finished.
    void
    tweenEndSlot();

    //! Releases static background image.
    static void
    releaseBG();
};

} /* namespace ilixi */
#endif /* NOTIFICATION_H_ */
