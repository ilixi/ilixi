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

#ifndef ILIXI_SURFACE_H_
#define ILIXI_SURFACE_H_

#include "types/Rectangle.h"
#include <string>

namespace ilixi
{
  //! Stores surface pixel data.
  class Surface
  {
    friend class Widget; // so it can release if necessary.
    friend class Painter; // access to dfbSurface

  public:
    /*!
     * Constructor.
     */
    Surface();

    /*!
     * Destructor.
     */
    virtual
    ~Surface();

    /*!
     * Creates a new DFB surface which has the same pixel format as window.
     *
     * @param width in pixels.
     * @param height in pixels.
     */

    bool
    createDFBSurface(int width, int height);

    /*!
     * Creates a new DFB sub-surface.
     *
     * @param geometry relative coordinates.
     * @param parent DFB surface.
     */
    bool
    createDFBSubSurface(const Rectangle& geometry, IDirectFBSurface* parent);

    /*!
     * Returns the interface to the underlying DirectFB surface.
     */
    IDirectFBSurface*
    DFBSurface();

    /*!
     * Returns the DirectFB surface ID, if there is no surface returns 0.
     */
    DFBSurfaceID
    DFBSurfaceId() const;

    /*!
     * This method is used for modifying the current geometry of DirectFB surface.
     *
     * @param geometry New surface geometry.
     */
    void
    setGeometry(const Rectangle& geometry);

    /*!
     * This method is used for modifying the current geometry of DirectFB surface.
     *
     * @param x
     * @param y
     * @param width
     * @param height
     */
    void
    setGeometry(int x, int y, int width, int height);

    /*!
     * Flips DFB surface.
     */
    void
    flip();

    /*!
     * Flips DFB surface.
     *
     * @param rect area to flip in surface coordinates.
     */
    void
    flip(const Rectangle& rect);

    /*!
     * Lock surface mutex. This is mainly used by Painter to serialise updates.
     */
    void
    lock();

    /*!
     * Unlocks surface mutex.
     */
    void
    unlock();

    /*!
     * Fills surface with RGBA(0,0,0,0), limited to the current clip.
     */
    void
    clear();

    /*!
     * Fills surface with RGBA(0,0,0,0), limited to the current clip.
     *
     * @param rect area to clear in surface coordinates.
     */
    void
    clear(const Rectangle& rect);

    /*!
     * Set the clipping region of DFB surface.
     *
     * @param rect area to clip.
     */
    void
    clip(const Rectangle& rect);

    /*!
     * Set the clipping region of DFB surface to NULL.
     */
    void
    resetClip();

    /*!
     * Blits given surface onto this surface.
     *
     * @param source DFB surface.
     * @param crop from source surface.
     * @param x coordinate of this surface.
     * @param y coordinate of this surface.
     */
    void
    blit(IDirectFBSurface* source, const Rectangle& crop, int x = 0, int y = 0);

    /*!
     * Blits given surface onto this surface.
     *
     * @param source DFB surface.
     * @param x coordinate of this surface.
     * @param y coordinate of this surface.
     */
    void
    blit(IDirectFBSurface* source, int x = 0, int y = 0);

    /*!
     * Blits given surface onto this surface.
     *
     * @param source DFB surface.
     * @param crop from source surface.
     * @param x coordinate of this surface.
     * @param y coordinate of this surface.
     */
    void
    blit(Surface* source, const Rectangle& crop, int x = 0, int y = 0);

    /*!
     * Blits given surface onto this surface.
     *
     * @param source DFB surface.
     * @param x coordinate of this surface.
     * @param y coordinate of this surface.
     */
    void
    blit(Surface* source, int x = 0, int y = 0);

  private:
    //! Interface to DFB surface.
    IDirectFBSurface* _dfbSurface;
    //! Interface to parent DFB surface.
    IDirectFBSurface* _parentSurface;

    //! This mutex is used for serialising writes to surface by Painter.
    pthread_mutex_t _surfaceLock;

    /*!
     * Release DirectFB surface.
     */
    void
    release();
  };
}
#endif /* ILIXI_SURFACE_H_ */
