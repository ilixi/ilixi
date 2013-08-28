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

#ifndef ILIXI_SURFACE_H_
#define ILIXI_SURFACE_H_

#include <types/Event.h>
#include <ilixiConfig.h>

namespace ilixi
{
class Widget;
//! Stores surface pixel data.
class Surface
{
    friend class Widget;    // so it can release if necessary.
    friend class Painter;   // access to dfbSurface

public:
    /*!
     * These flags specify surface geometry and allocation functionality.
     */
    enum SurfaceFlags
    {
        InitialiseSurface = 0x0001,                             //!< Set if widget should (re)initialise its surface.
        ModifiedPosition = 0x0002,                              //!< Widget position is modified.
        ModifiedSize = 0x0004,                                  //!< Widget size is modified.
        ModifiedGeometry = (ModifiedPosition | ModifiedSize),   //!< Widget's geometry is modified.
        DoZSort = 0x0008,                                       //!< Perform z-index sorting of children.
        HasOwnSurface = 0x0010,                                 //!< Widget has an independent surface and its surface is not a sub-surface of any parent widget.
        RootSurface = 0x0020,                                   //!< Widget is a WindowWidget and window surface is used directly.
        SubSurface = 0x0040,                                    //!< Widget uses a subsurface.
        SharedSurface = 0x0080,                                 //!< Widget uses window surface directly.
        DefaultDescription = (InitialiseSurface | ModifiedGeometry | SharedSurface),    //!< Default flags for widgets.
        BlitDescription = (InitialiseSurface | ModifiedGeometry | HasOwnSurface),       //!< Use if widget surface should be blitted onto another widget/surface, e.g. a widget inside a ScrollArea.
        WindowDescription = (InitialiseSurface | ModifiedGeometry | RootSurface)        //!< Use if widget is a WindowWidget, e.g. Application or Dialog.
    };

    /*!
     * Constructor.
     */
    Surface(Widget* owner);

    /*!
     * Destructor.
     */
    virtual
    ~Surface();

    /*!
     * Returns surface flags.
     *
     * @sa setSurfaceFlag();
     */
    SurfaceFlags
    flags() const;

    /*!
     * Returns the interface to the underlying DirectFB surface.
     */
    IDirectFBSurface*
    dfbSurface();

    /*!
     * Returns the DirectFB surface ID, if there is no surface returns 0.
     */
    DFBSurfaceID
    dfbSurfaceId() const;

    /*!
     * Sets a surface flag.
     */
    void
    setSurfaceFlag(SurfaceFlags flags);

    /*!
     * Unsets a surface flag.
     */
    void
    unsetSurfaceFlag(SurfaceFlags flags);

    /*!
     * This method is called within widget's paint method in order to initialise or
     * modify widget's surface and update the geometry of widget's frame and children.
     */
    void
    updateSurface(const PaintEvent& event);

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

    /*!
     * Sets surface opacity using drawing and blitting flags.
     */
    void
    setOpacity(u8 opacity);

#ifdef ILIXI_STEREO_OUTPUT
    bool
    createDFBSubSurfaceStereo(const Rectangle& geometry, IDirectFBSurface* parent, int zIndex);

    void
    setStereoGeometry(const Rectangle& geometry, int zIndex);

    void
    setStereoGeometry(int x, int y, int width, int height, int zIndex);

    PaintEvent::PaintEventEye
    stereoEye() const;

    void
    setStereoEye(PaintEvent::PaintEventEye eye);

    IDirectFBSurface*
    getStereoSurface(PaintEvent::PaintEventEye eye);

    void
    flipStereo(const Rectangle& left, const Rectangle& right);
#endif

protected:
    /*!
     * Creates a new DFB surface which has the same pixel format as window.
     *
     * @param width in pixels.
     * @param height in pixels.
     */
    bool
    createDFBSurface(int width, int height, DFBSurfaceCapabilities caps = DSCAPS_VIDEOONLY);

    /*!
     * Creates a new DFB sub-surface.
     *
     * @param geometry relative coordinates.
     * @param parent DFB surface.
     */
    bool
    createDFBSubSurface(const Rectangle& geometry, IDirectFBSurface* parent);

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

private:
    //! Pointer to owner widget.
    Widget* _owner;
    //! Interface to DFB surface.
    IDirectFBSurface* _dfbSurface;
    //! Interface to parent DFB surface.
    IDirectFBSurface* _parentSurface;
    /*!
     * This property controls the allocation of surface and specifies how a widget
     * frame and surface geometry should be used. It is set to DefaultDescription by default.
     */
    SurfaceFlags _flags;

#ifdef ILIXI_STEREO_OUTPUT
    IDirectFBSurface* _rightSurface;
    PaintEvent::PaintEventEye _eye;
#endif
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
