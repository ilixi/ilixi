/*
 * OSKComponent.cpp
 *
 *  Created on: Aug 8, 2012
 *      Author: tarik
 */

#include "OSKComponent.h"
#include <core/Logger.h>
#include "Compositor.h"

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_OSKCOMP, "ilixi/Coma/OSKComponent", "OSKComponent");

OSKComponent::OSKComponent(Compositor* compositor)
        : ComaComponent("PopUpComponent"),
          _compositor(compositor)
{
    init();
}

OSKComponent::~OSKComponent()
{
}

DirectResult
OSKComponent::comaMethod(ComaMethodID method, void *arg)
{
    DirectResult ret = DR_OK;

    switch (method)
    {
    case ShowOSK:
        {
            OSKRequest request = *((OSKRequest*) arg);
            ILOG_DEBUG(ILX_OSKCOMP, "ShowOSK\n");
            ILOG_DEBUG(ILX_OSKCOMP, " -> Mode (%d)\n", request.mode);
            ILOG_DEBUG(
                    ILX_OSKCOMP,
                    " -> Rectangle (%d, %d, %d, %d)\n", request.inputRect.x, request.inputRect.y, request.inputRect.w, request.inputRect.h);
            ILOG_DEBUG(ILX_OSKCOMP, " -> PID (%d) \n", request.process);
            // _compositor->showOSK(request.inputRect);
        }
        break;

    case HideOSK:
        ILOG_DEBUG(ILX_OSKCOMP, "HideOSK\n");
        // _compositor->hideOSK();
        break;

    default:
        ret = DR_NOIMPL;
        break;
    }
    return ret;
}

} /* namespace ilixi */
