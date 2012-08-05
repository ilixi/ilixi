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

#include "PopupComponent.h"
#include <core/Logger.h>
#include "Compositor.h"
#include "NotificationManager.h"

D_DEBUG_DOMAIN( ILX_POPUPCOMP, "ilixi/Coma/PopUpComp", "PopUpComponent");

namespace ilixi
{

PopupComponent::PopupComponent(Compositor* compositor)
        : ComaComponent("PopUpComponent"), _compositor(compositor), _notificationMan(
                NULL)
{
    init();
    _notificationMan = new NotificationManager(compositor);
}

PopupComponent::~PopupComponent()
{
    delete _notificationMan;
}

DirectResult
PopupComponent::comaMethod(ComaMethodID method, void *arg)
{
    DirectResult ret = DR_OK;
    unsigned int surfaceID = *((unsigned int*) arg);
    switch (method)
    {
    case AddNotification:
        ILOG_DEBUG(ILX_POPUPCOMP, "AddNotification for %u\n", surfaceID);
        _notificationMan->addNotification(surfaceID);
        break;

    case AddOverlay:
        ILOG_DEBUG(ILX_POPUPCOMP, "AddOverlay for %u\n", surfaceID);
        _compositor->addOverlay(surfaceID);
        break;

    case AddDialog:
        ILOG_DEBUG(ILX_POPUPCOMP, "AddDialog for %u\n", surfaceID);
        _compositor->addDialog(surfaceID);
        break;

    default:
        ret = DR_NOIMPL;
        break;
    }
    return ret;
}

} /* namespace ilixi */
