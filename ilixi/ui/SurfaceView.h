/*
 Copyright 2012 Tarik Sekmen.

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

#ifndef ILIXI_SURFACEVIEW_H_
#define ILIXI_SURFACEVIEW_H_

#include "ui/Widget.h"
#include "core/SurfaceEventListener.h"

namespace ilixi
{
  //! Renders any DFBSurface onto itself.
  /*!
   * This widget can be used to render the contents of a DFBSurface
   * directly onto itself.
   *
   * \warning Does not support sub-surfaces at the moment.
   */
  class SurfaceView : public SurfaceEventListener, virtual public Widget
  {
  public:
    /*!
     * Constructor
     * @param id of source surface
     * @param parent widget.
     */
    SurfaceView(DFBSurfaceID id = 0, Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~SurfaceView();

    /*!
     * Returns the size of source surface.
     */
    virtual ilixi::Size
    preferredSize() const;

    /*!
     * Returns the id of source surface.
     */
    DFBSurfaceID
    sourceID() const;

    /*!
     * Sets source surface using given id.
     */
    void
    setSourceFromSurfaceID(DFBSurfaceID id);

    /*!
     * Sets source surface to given surface.
     */
    void
    setSourceFromSurface(IDirectFBSurface* source);

    /*!
     * Sets source surface to window's surface.
     */
    void
    setSourceFromWindow(IDirectFBWindow* window);

    void
    paint(const Rectangle& targetArea);

    virtual bool
    consumeWindowEvent(DFBWindowEvent* event);

  protected:
    /*!
     * Scale ratio in horizontal direction.
     */
    float
    hScale() const;

    /*!
     * Scale ratio in vertical direction.
     */
    float
    vScale() const;

    /*!
     * Reimplement for decorations.
     */
    virtual void
    compose();

    /*!
     * Blits source surface on itself.
     */
    virtual void
    renderSource();

  private:
    //! This property stores the scale ratio in horizontal direction.
    float _hScale;
    //! This property stores the scale ratio in vertical direction.
    float _vScale;
    //! This property stores the window interface of source surface, if any.
    IDirectFBWindow* _window;

    void
    onSourceUpdate(const DFBSurfaceEvent& event);

    void
    onSourceDestroyed(const DFBSurfaceEvent& event);

    void
    onSVGeomUpdate();

  };

} /* namespace ilixi */
#endif /* ILIXI_SURFACEVIEW_H_ */
