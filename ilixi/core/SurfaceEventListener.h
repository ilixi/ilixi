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

#ifndef ILIXI_SURFACELISTENER_H_
#define ILIXI_SURFACELISTENER_H_

#include "directfb.h"

namespace ilixi
{

  //! Listens to incoming events from a surface.
  class SurfaceEventListener
  {
    friend class Window;

  public:
    /*!
     * Constructor will automatically add listener to Application.
     *
     * @param id source surface id
     */
    SurfaceEventListener(DFBSurfaceID id);

    /*!
     * Destructor will automatically remove listener from Application.
     */
    virtual
    ~SurfaceEventListener();

  protected:
    //! ID of source surface.
    DFBSurfaceID _id;

    //! This property stores source surface.
    IDirectFBSurface* _source;

    /*!
     * This method is called if source surface is updated.
     *
     * @param update Region to update in source surface coordinates.
     */
    virtual void
    onSourceUpdate(const DFBSurfaceEvent& event) = 0;

    /*!
     * This method is called if source surface is destroyed.
     *
     * @param update
     */
    virtual void
    onSourceDestroyed(const DFBSurfaceEvent& event) = 0;

    /*!
     * Attaches source surface to event buffer.
     */
    void
    attachEB();

    /*!
     * Detaches source surface from event buffer.
     */
    void
    detachEB();

  private:
    //! Intercepts surface events of source surface.
    bool
    consumeSurfaceEvent(const DFBSurfaceEvent& event);
  };

} /* namespace ilixi */
#endif /* ILIXI_SURFACELISTENER_H_ */
