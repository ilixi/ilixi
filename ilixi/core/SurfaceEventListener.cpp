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
#include "core/Window.h"
#include "core/Logger.h"

namespace ilixi
{

  SurfaceEventListener::SurfaceEventListener(DFBSurfaceID id) :
      _id(id), _source(NULL)
  {
    Window::addSurfaceEventListener(this);
  }

  SurfaceEventListener::~SurfaceEventListener()
  {
    Window::removeSurfaceEventListener(this);
  }

  void
  SurfaceEventListener::attachEB()
  {
    // FIXME lock window resources first!
    if (_source)
      {
        _source->MakeClient(_source);
//        _source->AllowAccess(_source, "*");
        _source->AttachEventBuffer(_source, Window::__buffer);
      }
  }

  void
  SurfaceEventListener::detachEB()
  {
    if (_source)
      _source->DetachEventBuffer(_source, Window::__buffer);
  }

  bool
  SurfaceEventListener::consumeSurfaceEvent(const DFBSurfaceEvent& event)
  {
    if (event.surface_id == _id)
      {
        if (event.type == DSEVT_DESTROYED)
          onSourceDestroyed(event);
        else if (event.type == DSEVT_UPDATE)
          onSourceUpdate(event);
        return true;
      }
    return false;
  }

} /* namespace ilixi */
