/*
 * PopupComponent.cpp
 *
 *  Created on: Jul 24, 2012
 *      Author: tarik
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
