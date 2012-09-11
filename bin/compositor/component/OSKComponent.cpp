/*
 * OSKComponent.cpp
 *
 *  Created on: Aug 8, 2012
 *      Author: tarik
 */

#include "OSKComponent.h"
#include "Compositor.h"
#include <core/ComponentData.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_OSKCOMP, "ilixi/Coma/OSKComponent", "OSKComponent");

OSKComponent::OSKComponent(ILXCompositor* compositor)
        : ComaComponent("OSK"),
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
    case OSK::ShowOSK:
        {
            OSK::OSKRequest request;
            if (arg == NULL)
            {
                request.inputRect =
                {   0,0,100,100};
            } else
            {
                request = *((OSK::OSKRequest*) arg);
                ILOG_DEBUG(ILX_OSKCOMP, "ShowOSK\n");
                ILOG_DEBUG(ILX_OSKCOMP, " -> Mode (%d)\n", request.mode);
                ILOG_DEBUG(
                        ILX_OSKCOMP,
                        " -> Rectangle (%d, %d, %d, %d)\n", request.inputRect.x, request.inputRect.y, request.inputRect.w, request.inputRect.h);
                ILOG_DEBUG(ILX_OSKCOMP, " -> PID (%d) \n", request.client);
            }
            _compositor->showOSK(request.inputRect, request.client);
        }
        break;

    case OSK::HideOSK:
        ILOG_DEBUG(ILX_OSKCOMP, "HideOSK\n");
        _compositor->toggleOSK(false);
        break;
    case OSK::ConsumeKey:
        {
            ILOG_DEBUG(ILX_OSKCOMP, "ConsumeKey\n");
            uint32_t key = *((uint32_t*) arg);
            ILOG_DEBUG(ILX_OSKCOMP, " -> U+%04X\n", key);
            _compositor->sendOSKInput(key);
        }
        break;
    default:
        ret = DR_NOIMPL;
        break;
    }
    return ret;
}

} /* namespace ilixi */
