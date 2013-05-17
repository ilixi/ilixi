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

#include <lib/Timer.h>
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
class AppBase
{
    friend class Application;
    friend class Callback;              // add/remove callback
    friend class SurfaceEventListener;  // add/remove SurfaceEventListener
    friend class Timer;                 // add/remove timer
    friend class Window;                // activeWindow
    friend class WindowWidget;
    friend class SurfaceView;

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
     * Post a custom user event.
     *
     * @param type of event [0-999].
     * @param data if not NULL, a pointer to your data.
     *
     * \warning make sure your event type is lower than 1000.
     */
    static void
    postUserEvent(unsigned int type, void* data = NULL);

    /*!
     * Post a universal event to main event buffer.
     *
     * @param target Widget.
     * @param type of event
     * @param data if not NULL, a pointer to your data.
     */
    static void
    postUniversalEvent(Widget* target, unsigned int type, void* data = NULL);

protected:
    /*!
     * This will post a key input event to active window.
     *
     * @param symbol unicode compatible key symbol.
     * @param modifierMask pressed modifiers, e.g. Shift.
     * @param lockState active locks
     * @param down true for KeyDown, false for KeyUp.
     */
    void
    postKeyEvent(DFBInputDeviceKeySymbol symbol, DFBInputDeviceModifierMask modifierMask, DFBInputDeviceLockState lockState, bool down = true);

    /*!
     * This will post a pointer input event to active window.
     *
     * @param type of pointer event, e.g. PointerMotion.
     * @param button button being pressed or released
     * @param buttonMask specifies currently pressed buttons
     * @param x position of window or coordinate within window
     * @param y position of window or coordinate within window
     * @param cx x cursor position
     * @param cy y cursor position
     * @param step wheel step
     */
    void
    postPointerEvent(PointerEventType type, PointerButton button, PointerButtonMask buttonMask, int x, int y, int cx, int cy, int step);

    /*!
     * Custom user events are handled in this function.
     * \sa postUserEvent
     */
    virtual void
    handleUserEvent(const DFBUserEvent& event);

    /*!
     * This method gets all window events first.
     */
    virtual bool
    windowPreEventFilter(const DFBWindowEvent& event);

    /*!
     * This enum is used to specify the state of an application.
     */
    enum AppFlags
    {
        APS_TERM = 0x0000001,       //!< Application is about to terminate shortly.
        APS_VISIBLE = 0x0000002,    //!< Application has a visible window and has access to events.
        APS_HIDDEN = 0x0000004,     //!< Application has no window and has no access to events.
        APS_CUSTOM = 0x0000008      //!< Disable waking up of buffer when an update is received.
    };

    /*!
     * Returns application flags.
     */
    AppFlags
    appFlags() const;

    /*!
     * Sets application flag.
     */
    void
    setAppFlag(AppFlags state);

    /*!
     * Clears application flag.
     */
    void
    clearAppFlags(AppFlags state);

    /*!
     * Returns current cursor position.
     */
    static DFBPoint
    cursorPosition();

    /*!
     * Executes each timer and returns a timeout for next interval in ms.
     */
    int32_t
    runTimers();

    /*!
     * Executes callbacks.
     */
    void
    runCallbacks();

    /*!
     * All events are handled using this function.
     *
     * @param timeout in ms.
     * @param forceWait causes to wait even if there is a pending UI update
     */
    void
    handleEvents(int32_t timeout, bool forceWait = false);

    /*!
     * Paints windows with affected areas.
     */
    void
    updateWindows();

private:
    //! Application state.
    AppFlags __flags;

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
    //! Serialises access to window list.
    pthread_mutex_t __windowMutex;

    typedef std::list<Timer*> TimerList;
    TimerList _timers;
    pthread_mutex_t __timerMutex;

    //! Event buffer for application.
    static IDirectFBEventBuffer* __buffer;
    //! AppBase instance.
    static AppBase* __instance;

    // Surface event sync.
    bool _update;
    bool _updateFromSurfaceView;
    Timer* _update_timer;
    DFBSurfaceID _updateID;
    unsigned int _updateFlipCount;
    long long _updateDiff;
    long long _updateTime;
    long long _updateDisable;

    void
    accountSurfaceEvent(const DFBSurfaceEvent& event, long long lastTime);

    void
    updateTimeout();

    void
    updateFromWindow();

    void
    initEventBuffer();

    void
    releaseEventBuffer();

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

    /*!
     * Adds a new timer to be monitored.
     */
    static bool
    addTimer(Timer* timer);

    /*!
     * Removes timer.
     */
    static bool
    removeTimer(Timer* timer);

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
     * Returns size of first window in stack.
     */
    static Size
    appSize();

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

protected:
    bool _syncWithSurfaceEvents;

    void
    disableSurfaceEventSync(long long micros);
};
}
#endif /* ILIXI_APPBASE_H_ */
