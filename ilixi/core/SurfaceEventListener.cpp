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

#include <core/SurfaceEventListener.h>

#include <core/Application.h>
#include <core/Engine.h>
#include <core/Logger.h>

namespace ilixi
{
D_DEBUG_DOMAIN( ILX_SURFACELISTENER_UPDATES, "ilixi/ui/SurfaceListener/Updates", "SurfaceEventListener");

SurfaceEventListener::SurfaceEventListener()
        : _surfaceID(0),
          _sourceSurface(NULL),
          _cb(this),
          _lastTime(0)
{
}

SurfaceEventListener::~SurfaceEventListener()
{
}

DFBSurfaceID
SurfaceEventListener::sourceID() const
{
    return _surfaceID;
}

IDirectFBSurface*
SurfaceEventListener::sourceSurface() const
{
    return _sourceSurface;
}

void
SurfaceEventListener::attachSourceSurface()
{
    Engine::instance().addSurfaceEventListener(this);
    _cb.start();
}

void
SurfaceEventListener::detachSourceSurface()
{
    _cb.stop();
    Engine::instance().removeSurfaceEventListener(this);
}

bool
SurfaceEventListener::consumeSurfaceEvent(const DFBSurfaceEvent& event)
{
    if (event.surface_id == _surfaceID)
    {
        ILOG_TRACE_F(ILX_SURFACELISTENER_UPDATES);
        if (event.type == DSEVT_DESTROYED)
            onSourceDestroyed(event);
        else if (event.type == DSEVT_UPDATE)
        {
            ILOG_DEBUG(ILX_SURFACELISTENER_UPDATES, " -> SURFEVT: receive id %d flip count %d\n", event.surface_id, event.flip_count);
            _queue.push(event);

            Application::__instance->accountSurfaceEvent(event, _lastTime);

            _lastTime = event.time_stamp;
        }
        return true;
    }
    return false;
}

bool
SurfaceEventListener::funck()
{
    if (!_queue.empty())
    {
        // FIXME: avoid queueing up too much
        while (_queue.size() > 2)
            _queue.pop();

        if (onSourceUpdate(_queue.front()))
            _queue.pop();
    }
    return true;
}

} /* namespace ilixi */
