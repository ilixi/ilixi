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

#include "CompositorComponent.h"
#include "Compositor.h"
#include "NotificationManager.h"
#include "ApplicationManager.h"
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_COMPCOMP, "ilixi/Coma/CompositorComponent",
               "CompositorComponent");

CompositorComponent::CompositorComponent(Compositor* compositor)
        : ComaComponent("CompositorComponent", CompositorNumNotifications),
          _compositor(compositor),
          _notificationMan(NULL)
{
    init();
    createNotification(AppVisible, NULL);
    createNotification(AppHidden, NULL);
    createNotification(AppStarting, NULL);
    createNotification(ShowingHome, NULL, CNF_NONE);
    createNotification(ShowingSwitcher, NULL, CNF_NONE);
    createNotification(HidingHome, NULL, CNF_NONE);
    createNotification(HidingSwitcher, NULL, CNF_NONE);
    createNotification(SendingAppList, NULL);
    createNotification(SoundVisible, NULL, CNF_NONE);
    createNotification(SoundHidden, NULL, CNF_NONE);
    createNotification(DashVisible, NULL, CNF_NONE);
    createNotification(DashHidden, NULL, CNF_NONE);
    _notificationMan = new NotificationManager(compositor);
}

CompositorComponent::~CompositorComponent()
{
    delete _notificationMan;
}

void
CompositorComponent::signalInstanceChanged(AppInstance* cur, AppInstance* pre)
{
    if (cur)
        notifyVisibility(cur, true);

    if (pre)
        notifyVisibility(pre, false);
}

void
CompositorComponent::signalHome(bool showing)
{
    if (showing)
        notify(ShowingHome, NULL);
    else
        notify(HidingHome, NULL);
}

void
CompositorComponent::signalSwitcher(bool showing)
{
    if (showing)
        notify(ShowingSwitcher, NULL);
    else
        notify(HidingSwitcher, NULL);
}

void
CompositorComponent::sendAppList()
{
    ILOG_TRACE_F(ILX_COMPCOMP);
    AppInfoList list = _compositor->appMan()->applicationList();
    std::vector<AppData> dataVector;

    for (AppInfoList::iterator it = list.begin(); it != list.end(); ++it)
    {
        if (!(((AppInfo*) *it)->appFlags() & APP_SYSTEM))
        {
            AppData data;
            snprintf(data.name, 64, "%s", ((AppInfo*) *it)->name().c_str());
            snprintf(data.icon, 256, "%s", ((AppInfo*) *it)->icon().c_str());
            dataVector.push_back(data);
        }
    }

    int* vec;
    allocate(sizeof(int) + dataVector.size() * sizeof(AppData), (void**) &vec);
    *vec = dataVector.size();
    memcpy(vec + 1, &dataVector[0], dataVector.size() * sizeof(AppData));
    notify(SendingAppList, vec);
    ILOG_DEBUG(ILX_COMPCOMP, "Sent application vector.\n");
}

void
CompositorComponent::signalSound(bool showing)
{
    if (showing)
        notify(SoundVisible, NULL);
    else
        notify(SoundHidden, NULL);
}

void
CompositorComponent::signalDash(bool showing)
{
    if (showing)
        notify(DashVisible, NULL);
    else
        notify(DashHidden, NULL);
}

void
CompositorComponent::signalAppStart(AppInstance* instance)
{
    VisibilityNotification vNo;
    snprintf(vNo.name, 64, "%s", instance->appInfo()->name().c_str());
    vNo.pid = instance->pid();
    vNo.multi = instance->appInfo()->appFlags() & APP_ALLOW_MULTIPLE;

    VisibilityNotification* tPid;
    allocate(sizeof(VisibilityNotification), (void**) &tPid);
    *tPid = vNo;

    notify(AppStarting, tPid);
}

DirectResult
CompositorComponent::comaMethod(ComaMethodID method, void *arg)
{
    DirectResult ret = DR_OK;
    switch (method)
    {
    case AddNotification:
        {
            Notify::NotifyData data = *((Notify::NotifyData*) arg);
            ILOG_DEBUG(ILX_COMPCOMP, "AddNotification for %s\n", data.sender);
            _notificationMan->addNotification(data);
            break;
        }

    case AddOverlay:
        {
            unsigned int surfaceID = *((unsigned int*) arg);
            ILOG_DEBUG(ILX_COMPCOMP, "AddOverlay for %u\n", surfaceID);
            _compositor->addOverlay(surfaceID);
            break;
        }

    case AddDialog:
        {
            unsigned int surfaceID = *((unsigned int*) arg);
            ILOG_DEBUG(ILX_COMPCOMP, "AddDialog for %u\n", surfaceID);
            _compositor->addDialog(surfaceID);
            break;
        }

    case ShowHome:
        _compositor->toggleLauncher(true);
        break;

    case ShowSwitcher:
        _compositor->toggleSwitcher(true);
        break;

    case HideHome:
        _compositor->toggleLauncher(false);
        break;

    case HideSwither:
        _compositor->toggleSwitcher(false);
        break;

    case StartApp:
        {
            char name[64];
            snprintf(name, 64, "%s", ((char*) arg));
            _compositor->appMan()->startApp(name);
        }
        break;

    case SendAppList:
        ILOG_TRACE_F(ILX_COMPCOMP);
        sendAppList();
        break;

    case SoundShow:
        _compositor->showSound(true);
        break;

    case SoundHide:
        _compositor->showSound(false);
        break;

    case DashShow:
        _compositor->showDash(true);
        break;

    case DashHide:
        _compositor->showDash(false);
        break;

    default:
        ret = DR_NOIMPL;
        break;
    }
    return ret;
}

void
CompositorComponent::notifyVisibility(AppInstance* instance, bool visible)
{
    VisibilityNotification vNo;
    snprintf(vNo.name, 64, "%s", instance->appInfo()->name().c_str());
    vNo.pid = instance->pid();
    vNo.multi = instance->appInfo()->appFlags() & APP_ALLOW_MULTIPLE;

    VisibilityNotification* tPid;
    allocate(sizeof(VisibilityNotification), (void**) &tPid);
    *tPid = vNo;

    if (visible)
    {
        notify(AppVisible, tPid);
        ILOG_DEBUG(ILX_COMPCOMP,
                   "%s is now visible!\n", instance->appInfo()->name().c_str());
    } else
    {
        notify(AppHidden, tPid);
        ILOG_DEBUG(ILX_COMPCOMP,
                   "%s is now hidden!\n", instance->appInfo()->name().c_str());
    }
}

} /* namespace ilixi */
