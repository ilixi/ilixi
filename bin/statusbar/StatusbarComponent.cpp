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

D_DEBUG_DOMAIN( ILX_STATCOMP, "ilixi/statusbar/StatComp", "StatusbarComponent");

StatusbarComponent::StatusbarComponent(StatusBar* statusbar)
        : ComaComponent("StatusbarComponent", StatCompNumNotifications),
          _statusbar(statusbar)
{
    ILOG_TRACE_F(ILX_STATCOMP);
    init();
    createNotification(0, NULL);
    createNotification(1, NULL);
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
    case GetOptimalSize:
        ILOG_DEBUG(ILX_STATCOMP, "GetOptimalSize %u\n", idValue);
        sendNotification(idValue);
        break;

    case AddContent:
        ILOG_DEBUG(ILX_STATCOMP, "AddContent %u\n", idValue);
        if (_statusbar->addRemoteContent(idValue))
            sendNotification(idValue);
        break;

    case RemoveContent:
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
    deallocate((void*) idValue);
}

void
StatusbarComponent::addContentDispatchCallback(void *ctx,
                                               ComaNotificationID notification,
                                               void *arg)
{
    ILOG_DEBUG(
            ILX_STATCOMP,
            "%s( %p, %lu, %p ) called!\n", __FUNCTION__, ctx, notification, arg);
}

void
StatusbarComponent::removeContentDispatchCallback(
        void *ctx, ComaNotificationID notification, void *arg)
{
    ILOG_DEBUG(
            ILX_STATCOMP,
            "%s( %p, %lu, %p ) called!\n", __FUNCTION__, ctx, notification, arg);
}

} /* namespace ilixi */
