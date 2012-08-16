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
    enum CompositorComaMethods
    {
        AddNotification = 0, // e.g. song changed.
        AddOverlay = 1, // No input
        AddDialog = 2,     // Alerts with OK button.
        ShowHome = 3,
        ShowSwitcher = 4,
        HideHome = 5,
        HideSwither = 6,
        StartApp = 7,
        SendAppList = 8
    };

    enum CompositorNotifications
    {
        AppVisible = 0,
        AppHidden = 1,
        AppHasFocus = 2,
        ShowingHome = 3,
        ShowingSwitcher = 4,
        HidingHome = 5,
        HidingSwitcher = 6,
        SendingAppList = 7,
        CompositorNumNotifications
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

    void
    signalHomeShowing();

    void
    signalSwitcherShowing();

    void
    signalHomeHidden();

    void
    signalSwitcherHidden();

    void
    sendAppList();

protected:
    virtual DirectResult
    comaMethod(ComaMethodID method, void *arg);

private:
    struct AppData
    {
        char name[128];
        char icon[256];
    };

    Compositor* _compositor;
    NotificationManager* _notificationMan;
};

} /* namespace ilixi */
#endif /* ILIXI_POPUPCOMPONENT_H_ */
