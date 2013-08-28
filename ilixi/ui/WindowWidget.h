/*
 Copyright 2010-2013 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>,
            Denis Oliver Kropp <dok@directfb.org>.

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

#ifndef ILIXI_WINDOWWIDGET_H_
#define ILIXI_WINDOWWIDGET_H_

#include <core/Window.h>
#include <core/EventManager.h>
#include <lib/Timer.h>
#include <ui/Frame.h>
#include <semaphore.h>
#include <vector>

namespace ilixi
{
//! Base class for windowed widgets.
class WindowWidget : public Frame
{
    friend class Application;
    friend class Surface;
    friend class Widget;
    friend class EventManager;
public:
    /*!
     * Constructor.
     */
    WindowWidget(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~WindowWidget();

    /*!
     * Returns true if window widget paints its background.
     */
    bool
    backgroundFilled() const;

    /*!
     * This method will queue a PaintEvent.
     */
    virtual void
    update();

    /*!
     * This method will queue the PaintEvent.
     */
    virtual void
    update(const PaintEvent& event);

    /*!
     * This method executes update().
     */
    virtual void
    doLayout();

    /*!
     * Paints inside given rectangle.
     *
     * Actual painting takes place once a union rectangle is
     * formed by updateWindow or repaint methods.
     */
    virtual void
    paint(const PaintEvent& event);

    /*!
     * Immediately repaints inside given rectangle.
     */
    virtual void
    repaint(const PaintEvent& event);

    /*!
     * Sets whether window widget paints its background.
     */
    void
    setBackgroundFilled(bool fill);

    /*!
     * Sets whether window widget clears before painting.
     */
    void
    setBackgroundClear(bool clear);

    virtual bool
    consumePointerEvent(const PointerEvent& pointerEvent);

protected:
    enum BackgroundFlags
    {
        BGFNone = 0x000,
        BGFClear = 0x001,
        BGFFill = 0x002
    };

    //! This flag specifies whether frame fills its background.
    char _backgroundFlags;

    /*!
     * Makes this widget and its window visible.
     */
    void
    showWindow();

    /*!
     * Sets visibility to false and releases window and surface resources of all children.
     */
    void
    closeWindow();

    /*!
     * Returns the unique DirectFB window ID.
     */
    DFBWindowID
    windowID() const;

    /*!
     * Handles incoming input events if window has focus.
     * Behaviour is dependant on incoming event type. Key events are sent to focused widget.
     * Pointer events are consumed by the widget which contains the event coordinates. If two or more
     * widgets contain the event coordinates, event is sent to the widget at the top.
     *
     * @param event parsed by DirectFB interfaces.
     * @return True if event is consumed.
     */
    bool
    handleWindowEvent(const DFBWindowEvent& event);

    /*!
     * This signal is emitted when window is aborted/closed.
     */
    sigc::signal<void> sigAbort;

private:
    //! This property stores the managed window.
    Window* _window;
    //! This property stores the event manager for this window.
    /*!
     * The event manager is owned by AppBase once this window is attached to it.
     */
    EventManager* _eventManager;

    std::string _layerName;

    class UpdateQueue
    {
    public:
        Rectangle rect;

        bool valid;

        UpdateQueue()
                : valid(false)
        {
        }

        void
        add(const Rectangle &ext)
        {
            if (valid)
            {
                rect = rect.united(ext);
            } else
            {
                rect = ext;
                valid = true;
            }
        }

        void
        reset()
        {
            valid = false;
        }
    };

    //! Stores window's dirty regions and a region for update.
    struct
    {
        pthread_mutex_t _listLock;
        sem_t _updateReady;
        sem_t _paintReady;
        Rectangle _updateRegion;
#ifdef ILIXI_STEREO_OUTPUT
        Rectangle _updateRegionRight;
        UpdateQueue _updateQueueRight;
#endif
        UpdateQueue _updateQueue;
    } _updates;

    static IDirectFBSurface* _exclusiveSurface;

    /*!
     * Creates a united rectangle from a list of dirty regions and
     * performs a paint operation on this united rectangle.
     */
    virtual void
    updateWindow();

    IDirectFBSurface*
    windowSurface();

    std::string
    layerName() const;
};

}

#endif /* ILIXI_WINDOWWIDGET_H_ */
