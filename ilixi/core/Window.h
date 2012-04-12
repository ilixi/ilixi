/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#ifndef ILIXI_WINDOW_H_
#define ILIXI_WINDOW_H_

#include "core/EventManager.h"
#include "core/SurfaceEventListener.h"
#include "core/Callback.h"
#include "types/Size.h"
#include "lib/TweenAnimation.h"

namespace ilixi
{
  //! Creates a DirectFB window.
  /*!
   * Each application has at least one window and this class creates and maintains a list of DirectFB windows.
   * Windows also store a list of child widgets using UIManager which helps to transfer input events.
   */
  class Window
  {
    friend class Application;
    friend class Image;
    friend class Video;
    friend class Surface;
    friend class Font;
    friend class Callback;
    friend class Widget;
    friend class SurfaceView; // TODO Remove this line!
    friend class SurfaceEventListener;

  public:

    /*!
     * Creates a window with its own event manager.
     */
    Window();

    /*!
     * Destructor.
     */
    virtual
    ~Window();

    /*!
     * Returns the unique DirectFB window ID.
     */
    DFBWindowID
    getWindowID() const;

    /*!
     * Returns DirectFB window interface.
     */
    IDirectFBSurface*
    getDFBSurface() const;

    /*!
     * Returns the size of window.
     */
    Size
    getWindowSize() const;

    /*!
     *  Makes the window visible and starts input event thread.
     */
    void
    showWindow();

    /*!
     * Hides the window and terminates input event thread.
     */
    void
    hideWindow();

  protected:
    //! This property stores an interface to DirectFB window.
    IDirectFBWindow* _window;
    //! This property stores an interface to window's surface.
    IDirectFBSurface* _windowSurface;
    //! This property stores the event manager for this window.
    EventManager* _eventManager;

    // Fixme move this stuff to window manager code.
    //! This animation starts when a dialog is ready to show itself.
    TweenAnimation* _dialogShow;
    //! This animation starts when a dialog is ready to hide itself.
    TweenAnimation* _dialogHide;
    //! This property is eased during animations.
    float _aniValue;
    float _aniValue2;
    //! Bounding rectangle of window.
    Rectangle _bounds;

    /*!
     * Initialise DirectFB. This method is executed
     * only once by main Application during its construction.
     */
    static bool
    initDFB(int argc, char **argv, AppOptions opts);

    /*!
     * Creates a DFBWindow and acquires an interface to its surface.
     */
    bool
    initDFBWindow(const Size& size);

    /*!
     * Release DirectFB window interfaces.
     */
    void
    releaseDFBWindow();

    /*!
     * Returns DirectFB interface.
     */
    static IDirectFB*
    getDFB();

  private:
    //! This property stores parent window, if any.
    Window* _parentWindow;

    typedef std::list<Callback*> CallbackList;
    //! List of callbacks
    static CallbackList __callbacks;
    //! Serialises access to __callbacks.
    static pthread_mutex_t __cbMutex;

    typedef std::list<SurfaceEventListener*> SurfaceListenerList;
    //! List of surface event listeners.
    static SurfaceListenerList __selList;
    //! Serialises access to _selList.
    static pthread_mutex_t __selMutex;

    //! DirectFB interface is initialised by first window.
    static IDirectFB* __dfb;
    //! Pointer to DFBLayer.
    static IDirectFBDisplayLayer* __layer;
    //! Event buffer for window.
    static IDirectFBEventBuffer* __buffer;

    typedef std::list<Window*> WindowList;
    //! Application wide list of windows.
    static WindowList __windowList;
    //! Window with focus.
    static Window* __activeWindow;
    //! Serialises access to static variables.
    static pthread_mutex_t __windowMutex;

    //! Perform window animations.
    void
    animateWindow();

    /*!
     * Releases DirectFB resources.
     */
    static void
    releaseDFB();

    /*!
     * Returns active window.
     */
    static Window*
    activeWindow();

    static void
    addCallback(Callback* cb);

    static bool
    removeCallback(Callback* cb);

    static void
    handleCallbacks();

    static bool
    addSurfaceEventListener(SurfaceEventListener* sel);

    static bool
    removeSurfaceEventListener(SurfaceEventListener* sel);

    /*!
     * Sends incoming surface event to a surface event listener object.
     */
    static void
    handleSurfaceEvent(const DFBSurfaceEvent& event);

    /*!
     * Implemented in WindowWidget.
     */
    virtual bool
    handleWindowEvent(const DFBWindowEvent& event) =0;

    /*!
     * Implemented in WindowWidget.
     */
    virtual void
    updateWindow() =0;

  };

}

#endif /* ILIXI_WINDOW_H_ */
