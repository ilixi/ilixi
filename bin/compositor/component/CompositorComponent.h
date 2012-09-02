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
#include "AppInstance.h"

namespace ilixi
{

class Compositor;
class NotificationManager;

class CompositorComponent : public ilixi::ComaComponent
{
public:
    enum CompositorComaMethods
    {
        AddNotification = 0,    // e.g. song changed.
        AddOverlay = 1,         // No input
        AddDialog = 2,          // Alerts with OK button.
        ShowHome = 3,
        ShowSwitcher = 4,
        HideHome = 5,
        HideSwither = 6,
        StartApp = 7,
        SendAppList = 8,
        SoundShow = 9,
        SoundHide = 10,
        DashShow = 11,
        DashHide = 12,
        SendBackKey = 13
    };

    enum CompositorNotifications
    {
        AppVisible = 0,
        AppHidden = 1,
        AppStarting = 2,
        ShowingHome = 3,
        ShowingSwitcher = 4,
        HidingHome = 5,
        HidingSwitcher = 6,
        SendingAppList = 7,
        SoundVisible = 8,
        SoundHidden = 9,
        DashVisible = 10,
        DashHidden = 11,
        BackVisible = 12,
        BackHidden = 13,
        CompositorNumNotifications
    };

    CompositorComponent(Compositor* compositor);

    virtual
    ~CompositorComponent();

    void
    signalInstanceChanged(AppInstance* current, AppInstance* previous);

    void
    signalHome(bool showing);

    void
    signalSwitcher(bool showing);

    void
    sendAppList();

    void
    signalSound(bool showing);

    void
    signalDash(bool showing);

    void
    signalAppStart(AppInstance* instance);

    void
    signalBack(bool showing);

    void
    notifyVisibility(AppInstance* instance, bool visible);

protected:
    virtual DirectResult
    comaMethod(ComaMethodID method, void *arg);

private:
    struct AppData
    {
        char name[64];
        char icon[256];
    };

    struct VisibilityNotification
    {
        char name[64];
        pid_t pid;bool multi;
    };

    Compositor* _compositor;
    NotificationManager* _notificationMan;
};

} /* namespace ilixi */
#endif /* ILIXI_POPUPCOMPONENT_H_ */
