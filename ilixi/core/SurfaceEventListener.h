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

#ifndef ILIXI_SURFACELISTENER_H_
#define ILIXI_SURFACELISTENER_H_

#include <directfb.h>
#include <queue>
#include <core/Callback.h>

namespace ilixi
{

//! Listens to incoming events from a surface.
class SurfaceEventListener : public Functionoid
{
    friend class Engine;

public:
    /*!
     * Constructor will automatically add listener to Application.
     */
    SurfaceEventListener();

    /*!
     * Destructor will automatically remove listener from Application.
     */
    virtual
    ~SurfaceEventListener();

    /*!
     * Returns the id of source surface.
     */
    DFBSurfaceID
    sourceID() const;

    /*!
     * Returns the source surface.
     */
    IDirectFBSurface*
    sourceSurface() const;

protected:
    //! ID of source surface.
    DFBSurfaceID _surfaceID;

    //! This property stores source surface.
    IDirectFBSurface* _sourceSurface;

    /*!
     * Attach source surface to application event buffer.
     */
    void
    attachSourceSurface();

    /*!
     * Detach source surface from application event buffer.
     */
    void
    detachSourceSurface();

    /*!
     * This method is called if source surface is updated.
     *
     * @param update Region to update in source surface coordinates.
     */
    virtual bool
    onSourceUpdate(const DFBSurfaceEvent& event) = 0;

    /*!
     * This method is called if source surface is destroyed.
     *
     * @param update
     */
    virtual void
    onSourceDestroyed(const DFBSurfaceEvent& event) = 0;

private:
    //! Callback for stack.
    Callback _cb;
    //! Stack of surface events waiting.
    std::queue<DFBSurfaceEvent> _queue;

    //! Intercepts surface events of source surface.
    bool
    consumeSurfaceEvent(const DFBSurfaceEvent& event);

    //! Functor of callback.
    bool
    funck();

    long long _lastTime;
};

} /* namespace ilixi */
#endif /* ILIXI_SURFACELISTENER_H_ */
