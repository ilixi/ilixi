/*
 Copyright 2010-2012 Tarik Sekmen.

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

#ifndef ILIXI_APPBASE_H_
#define ILIXI_APPBASE_H_

#include <lib/Util.h>
#include <types/Enums.h>
#include <core/EventManager.h>
#include <core/Callback.h>
#include <core/Window.h>
#include <list>
#include <map>
#include <vector>
#include <stdint.h>
#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
#include <core/SurfaceEventListener.h>
#endif

namespace ilixi
{
class WindowWidget;
class Timer;

//! Base class for ilixi applications.
/*!
 */
class AppBase
{
    friend class WindowWidget;
    friend class Window;

    friend class Timer;

    friend class EventManager;
    friend class Image;
    friend class Video;
    friend class Surface;
    friend class Font;
    friend class FontCache;
    friend class Callback;
    friend class Widget;
    friend class SurfaceEventListener;
    friend class SurfaceView;
    friend class Application;
    friend class WebView;

    /*!
     * This enum is used to specify the state of an application.
     */
    enum AppState
    {
        APS_TERM = 0x0000001, //!< Application is about to terminate shortly.
        APS_VISIBLE = 0x0000002, //!< Application has a visible window and has access to events.
        APS_HIDDEN = 0x0000004, //!< Application has no window and has no access to events.
    };

public:
    /*!
     * Constructor.
     */
    AppBase(int* argc, char*** argv, AppOptions options);

    /*!
     * Destructor.
     */
    virtual
    ~AppBase();

    /*!
     * Returns title of application.
     */
    static std::string
    title();

    /*!
     * Sets application title.
     */
    void
    setTitle(std::string title);

    /*!
     * Returns EventBuffer interface.
     */
    static IDirectFBEventBuffer*
    getEventBuffer();

    /*!
     * Post a universal event to main event buffer.
     *
     * @param target Widget.
     * @param type of event
     * @param data if not NULL.
     */
    static void
    postUniversalEvent(Widget* target, unsigned int type, void* data = NULL);

protected:
    /*!
     * User events are handled before other event types.
     */
    virtual void
    handleUserEvent(const DFBUserEvent& event);

    /*!
     * This method gets window events first.
     */
    virtual bool
    windowPreEventFilter(const DFBWindowEvent& event);

    /*!
     * Sets application state flag.
     */
    void
    setAppState(AppState state);

    /*!
     * Clears application state flag.
     */
    void
    clearAppState(AppState state);

    /*!
     * Returns DirectFB interface.
     */
    static IDirectFB*
    getDFB();

    /*!
     * Returns DisplayLayer interface.
     */
    static IDirectFBDisplayLayer*
    getLayer();

    IDirectFBWindow*
    activeDFBWindow() const;

    static AppOptions
    appOptions();

    static void
    setAppOption(AppOptions option);

    static void
    unSetAppOption(AppOptions option);

    static DFBPoint
    cursorPosition();

    void
    setLayerSize(int width, int height);

private:
    //! This property stores various options for application behaviour.
    AppOptions __options;
    //! Application title.
    std::string __title;
    //! Application state.
    AppState __state;

    DFBPoint __cursorOld, __cursorNew;
    DFBDimension __layerSize;

    typedef std::list<Callback*> CallbackList;
    //! List of callbacks
    CallbackList __callbacks;
    //! Serialises access to __callbacks.
    pthread_mutex_t __cbMutex;

#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
    typedef std::list<SurfaceEventListener*> SurfaceListenerList;
    //! List of surface event listeners.
    SurfaceListenerList __selList;
    //! Serialises access to _selList.
    pthread_mutex_t __selMutex;
#endif

    typedef std::list<WindowWidget*> WindowList;
    //! Application wide list of windows.
    WindowList __windowList;
    //! Window with focus.
    WindowWidget* __activeWindow;
    //! Serialises access to static variables.
    pthread_mutex_t __windowMutex;

    typedef std::vector<Timer*> TimerList;
    TimerList _timers;
    pthread_mutex_t __timerMutex;

    //! DirectFB interface.
    static IDirectFB* __dfb;
    //! DFBLayer interface.
    static IDirectFBDisplayLayer* __layer;
    //! Event buffer for application.
    static IDirectFBEventBuffer* __buffer;
    //! AppBase instance.
    static AppBase* __instance;

    /*!
     * Initialise DirectFB. This method is executed
     * only once by main Application during its construction.
     */
    void
    initDFB(int* argc, char*** argv);

    /*!
     * Releases DirectFB resources.
     */
    void
    releaseDFB();

    /*!
     * Adds callback.
     */
    static bool
    addCallback(Callback* cb);

    /*!
     * Removes callback.
     */
    static bool
    removeCallback(Callback* cb);

    static bool
    addTimer(Timer* timer);

    static bool
    removeTimer(Timer* timer);

    /*!
     * Executes each callback and returns a timeout in ms.
     */
    int32_t
    runCallbacks();

#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
    /*!
     * Adds surface event listener.
     */
    static bool
    addSurfaceEventListener(SurfaceEventListener* sel);

    /*!
     * Removes surface event listener.
     */
    static bool
    removeSurfaceEventListener(SurfaceEventListener* sel);

    /*!
     * Forwards incoming surface event to corresponding surface event listener object.
     */
    void
    consumeSurfaceEvent(const DFBSurfaceEvent& event);
#endif

    /*!
     * Returns active window.
     */
    static WindowWidget*
    activeWindow();

    /*!
     * Sets active window.
     */
    static void
    setActiveWindow(WindowWidget* window);

    /*!
     * Adds window to list of managed windows.
     */
    static bool
    addWindow(WindowWidget* window);

    /*!
     * Removes window from list of managed windows.
     */
    static bool
    removeWindow(WindowWidget* window);

    /*!
     * Paints windows with affected areas.
     */
    void
    updateWindows();

    /*!
     * All events are handled using this function.
     */
    void
    handleEvents(int32_t timeout);

    /*!
     * Attach window to event buffer.
     */
    static void
    attachDFBWindow(Window* window);

    /*!
     * Detach window from event buffer.
     */
    static void
    detachDFBWindow(Window* window);

    void
    handleKeyInputEvent(const DFBInputEvent& event, DFBWindowEventType type);

    void
    handleButtonInputEvent(const DFBInputEvent& event, DFBWindowEventType type);

    void
    handleButtonRelease(const DFBInputEvent& event);

    void
    handleAxisMotion(const DFBInputEvent& event);

};
}
#endif /* ILIXI_APPBASE_H_ */
