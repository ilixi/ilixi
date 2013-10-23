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

#ifndef ILIXI_DRAGHELPER_H_
#define ILIXI_DRAGHELPER_H_

#include <types/Image.h>
#include <ui/WindowWidget.h>

#if ILIXI_HAVE_CAIRO
#include <cairo.h>
#endif

namespace ilixi
{
/*!
 * Creates a drag and drop context.
 *
 * API is still experimental and it can change in the future.
 */
class DragHelper : public WindowWidget
{
public:
    DragHelper(Widget* owner);

    virtual
    ~DragHelper();

    virtual Size
    preferredSize() const;

    void*
    data() const;

    const Point&
    hotspot() const;

    void
    setData(void* data);

    void
    setHotspot(const Point& hotspot);

    void
    setImage(Image* image, const Size& size = Size());

    void
    setImageFromDFBSurface(IDirectFBSurface* surface, const Size& size = Size());

#if ILIXI_HAVE_CAIRO
    void
    setImageFromCairoSurface(cairo_surface_t* surface);
#endif
    void
    startDrag(int x, int y);

    /*!
     * This signal is emitted when DragHelper is dropped.
     */
    sigc::signal<void, Point> sigDragEnded;

protected:
    virtual bool
    handleWindowEvent(const DFBWindowEvent& event, bool dragging = false);

private:
    Widget* _owner;
    void* _data;
    IDirectFBSurface* _surface;
    Point _hotSpot;

    void
    compose(const PaintEvent& event);

    void
    releaseDragHelper();
};

} /* namespace ilixi */
#endif /* ILIXI_DRAGHELPER_H_ */
