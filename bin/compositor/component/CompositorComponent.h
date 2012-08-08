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

#ifndef ILIXI_POPUPCOMPONENT_H_
#define ILIXI_POPUPCOMPONENT_H_

#include <core/ComaComponent.h>

namespace ilixi
{

class Compositor;
class NotificationManager;

class CompositorComponent : public ilixi::ComaComponent
{
public:
    enum PopupMethods
    {
        AddNotification, // e.g. song changed.
        AddOverlay, // No input
        AddDialog     // Alerts with OK button.
    };

    enum CompositorNotifications
    {
        AppVisible, AppHidden, AppHasFocus, CompositorNumNotifications
    };

    CompositorComponent(Compositor* compositor);

    virtual
    ~CompositorComponent();

    void
    notifyVisible(pid_t pid);

    void
    notifyHidden(pid_t pid);

    void
    notifyHasFocus(pid_t pid);

protected:
    virtual DirectResult
    comaMethod(ComaMethodID method, void *arg);

private:
    Compositor* _compositor;
    NotificationManager* _notificationMan;
};

} /* namespace ilixi */
#endif /* ILIXI_POPUPCOMPONENT_H_ */
