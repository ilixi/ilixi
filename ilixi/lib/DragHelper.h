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
//! Creates a drag and drop context.
/*!
 * API is still experimental and it can change in the future.
 */
class DragHelper : public WindowWidget
{
public:
    /*!
     * This enum specifies a list of valid drag and drop operations.
     */
    enum DragOperation
    {
        DragOperationNone = 0,
        DragOperationCopy = 1,
        DragOperationLink = 2,
        DragOperationGeneric = 4,
        DragOperationPrivate = 8,
        DragOperationMove = 16,
        DragOperationDelete = 32,
        DragOperationEvery = UINT_MAX
    };

    /*!
     * Constructor
     */
    DragHelper(Widget* owner);

    /*!
     * Destructor
     */
    virtual
    ~DragHelper();

    /*!
     * Returns size of image surface
     */
    virtual Size
    preferredSize() const;

    /*!
     * Returns data for drag operation
     */
    void*
    data() const;

    /*!
     * Returns the position of hot spot for drag window
     */
    const Point&
    hotspot() const;

    /*!
     * Returns the drag operation type of last drag and drop.
     */
    static DragOperation
    dragOperation();

    /*!
     * Sets the data for the drag operation.
     */
    void
    setData(void* data);

    /*!
     * Sets the position of hot spot for drag window.
     */
    void
    setHotspot(const Point& hotspot);

    /*!
     * Sets image for drag operation.
     *
     * Image is shown inside a window during drag.
     */
    void
    setImage(Image* image, const Size& size = Size());

    /*!
     * Sets image for drag operation using DirectFB surface.
     *
     * Image is shown inside a window during drag.
     */
    void
    setImageFromDFBSurface(IDirectFBSurface* surface, const Size& size = Size());

#if ILIXI_HAVE_CAIRO
    /*!
     * Sets image for drag operation using a Cairo surface.
     */
    void
    setImageFromCairoSurface(cairo_surface_t* surface);
#endif
    /*!
     * Starts drag operation.
     */
    void
    startDrag(int x, int y);

    /*!
     * Sets the type of drag operation.
     */
    static void
    setDragOperation(DragOperation operation);

    /*!
     * This signal is emitted when DragHelper is dropped.
     */
    sigc::signal<void, Point> sigDragEnded;

protected:
    /*!
     * This method moves drag window and notifies application when drag ends.
     */
    virtual bool
    handleWindowEvent(const DFBWindowEvent& event, bool dragging = false);

private:
    //! This property stores the widget which owns drag operation.
    Widget* _owner;
    //! This property stores the data for drag operation.
    void* _data;
    //! This property stores a pointer to drag image.
    IDirectFBSurface* _surface;
    //! This property stores the position of hot spot.
    Point _hotSpot;
    //! This property specifies the type of drag operation.
    DragOperation _operation;
    static DragHelper* __instance;

    void
    compose(const PaintEvent& event);

    void
    releaseDragHelper();
};

} /* namespace ilixi */
#endif /* ILIXI_DRAGHELPER_H_ */
