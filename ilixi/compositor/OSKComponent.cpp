/*
 Copyright 2010-2013 Tarik Sekmen.

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

#include <compositor/OSKComponent.h>
#include <compositor/Compositor.h>
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
                request.inputRect.x = 0;
                request.inputRect.y = 0;
                request.inputRect.w = 100;
                request.inputRect.h = 100;
            } else
            {
                request = *((OSK::OSKRequest*) arg);
                ILOG_DEBUG(ILX_OSKCOMP, "ShowOSK\n");
                ILOG_DEBUG(ILX_OSKCOMP, " -> Mode (%d)\n", request.mode);
                ILOG_DEBUG(ILX_OSKCOMP, " -> Rectangle (%d, %d, %d, %d)\n", request.inputRect.x, request.inputRect.y, request.inputRect.w, request.inputRect.h);
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
            uint32_t key = ((uint32_t*) arg)[0];
            uint32_t mask = ((uint32_t*) arg)[1];
            ILOG_DEBUG(ILX_OSKCOMP, " -> U+%04X\n", key);
            _compositor->sendOSKInput(key, mask);
        }
        break;
    default:
        ret = DR_NOIMPL;
        break;
    }
    return ret;
}

} /* namespace ilixi */
