/*
 Copyright 2011 Tarik Sekmen.

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

#include "core/SurfaceEventListener.h"
#include "core/AppBase.h"
#include "core/Logger.h"

namespace ilixi
{

  SurfaceEventListener::SurfaceEventListener() :
      _surfaceID(0), _sourceSurface(NULL), _cb(this)
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
    AppBase::addSurfaceEventListener(this);
    _cb.start();
  }

  void
  SurfaceEventListener::detachSourceSurface()
  {
    _cb.stop();
    AppBase::removeSurfaceEventListener(this);
  }

  bool
  SurfaceEventListener::consumeSurfaceEvent(const DFBSurfaceEvent& event)
  {
    if (event.surface_id == _surfaceID)
      {
        if (event.type == DSEVT_DESTROYED)
          onSourceDestroyed(event);
        else if (event.type == DSEVT_UPDATE)
          _stack.push(event);
        return true;
      }
    return false;
  }

  bool
  SurfaceEventListener::funck()
  {
    if (!_stack.empty())
      {
        onSourceUpdate(_stack.top());
        _stack.pop();
      }
    return true;
  }

} /* namespace ilixi */
