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

#include <core/Service.h>

#include <core/Engine.h>
#include <core/Logger.h>
#include <core/PlatformManager.h>
#include <vector>


namespace ilixi
{

Service* Service::__instance = NULL;

D_DEBUG_DOMAIN( ILX_SERVICE, "ilixi/core/Service", "Service");

Service::Service(int* argc, char*** argv, AppOptions opts)
{
    ILOG_TRACE_F(ILX_SERVICE);

    if (__instance)
        ILOG_THROW(ILX_SERVICE, "Cannot allow more than one instance!\n");

    __instance = this;

    PlatformManager::instance().initialize(argc, argv, opts);
    Engine::instance().initialise();
}

Service::~Service()
{
    ILOG_TRACE_F(ILX_SERVICE);
    Engine::instance().release();
    PlatformManager::instance().release();

    __instance = NULL;
}

void
Service::exec()
{
    ILOG_INFO(ILX_SERVICE, "Starting...\n");

    while (true)
    {
        if (Engine::instance().stopped())
            break;
        else
            __instance->handleEvents(Engine::instance().cycle());
    }

    ILOG_INFO(ILX_SERVICE, "Stopping...\n");
}

void
Service::quit()
{
    ILOG_TRACE_F(ILX_SERVICE);
    Engine::instance().stop();
}

void
Service::handleUserEvent(const DFBUserEvent& event)
{
}

void
Service::handleEvents(int32_t timeout, bool forceWait)
{
    ILOG_TRACE_F(ILX_SERVICE);

    Engine::instance().waitForEvents(timeout);
    DFBEvent event;
    while (Engine::instance().getNextEvent(&event) == DFB_OK)
    {
        switch (event.clazz)
        {
        case DFEC_USER:
            handleUserEvent((const DFBUserEvent&) event);
            break;

        default:
            break;
        }
    }

    ILOG_DEBUG(ILX_SERVICE, " -> end handle events \n");
}

} /* namespace ilixi */
