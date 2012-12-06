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

#include "StatusbarComponent.h"
#include "StatusBar.h"
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_STATCOMP, "ilixi/car/status/StatComp", "StatusbarComponent");

StatusbarComponent::StatusbarComponent(ILXStatusBar* statusbar)
        : ComaComponent("StatusBar", StatusBar::SBNumNotifications),
          _statusbar(statusbar)
{
    ILOG_TRACE_F(ILX_STATCOMP);
    init();
    createNotification(StatusBar::ContentAdded, NULL);
    createNotification(StatusBar::ContentRemoved, NULL);
}

StatusbarComponent::~StatusbarComponent()
{
    ILOG_TRACE_F(ILX_STATCOMP);
}

DirectResult
StatusbarComponent::comaMethod(ComaMethodID method, void *arg)
{
    DirectResult ret = DR_OK;
    DFBSurfaceID idValue = *((DFBSurfaceID*) arg);

    switch (method)
    {
    case StatusBar::AddContent:
        ILOG_DEBUG(ILX_STATCOMP, "AddContent %u\n", idValue);
        if (_statusbar->addRemoteContent(idValue))
            sendNotification(idValue);
        break;

    case StatusBar::RemoveContent:
        ILOG_DEBUG(ILX_STATCOMP, "RemoveContent %u\n", idValue);
        if (_statusbar->removeRemoteContent(idValue))
            sendNotification(idValue);
        break;

    default:
        ret = DR_NOIMPL;
        break;
    }
    return ret;
}

void
StatusbarComponent::sendNotification(DFBSurfaceID id)
{
    DFBSurfaceID* idValue;
    allocate(sizeof(idValue), (void**) &idValue);
    *idValue = id;
    notify(0, idValue);
    ILOG_DEBUG(ILX_STATCOMP, "Notification sent for surface %d\n", id);
}

} /* namespace ilixi */
