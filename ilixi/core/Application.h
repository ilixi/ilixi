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

#ifndef ILIXI_APPLICATION_H_
#define ILIXI_APPLICATION_H_

#include <lib/Util.h>
#include <core/Callback.h>
#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
#include <core/SurfaceEventListener.h>
#endif
#include <ui/AppWindow.h>

namespace ilixi
{

//! Windowed application.
/*!
 * This class is used to create a new UI application with its own window.
 */
class Application
{
public:
    /*!
     * Constructor.
     */
    Application(int* argc, char*** argv, AppOptions opts = OptNone);

    /*!
     * Destructor.
     */
    virtual
    ~Application();

    /*!
     * Returns application window width in pixels.
     */
    int
    width() const;

    /*!
     * Returns application window height in pixels.
     */
    int
    height() const;

    /*!
     * Adds a new child widget to application window.
     *
     * \sa ContainerBase::addWidget()
     */
    bool
    addWidget(Widget* widget);

    /*!
     * Removes a child widget from container.
     *
     * \sa addWidget()
     */
    bool
    removeWidget(Widget* widget);

    /*!
     * Updates application window.
     */
    void
    update();

    /*!
     * Terminates application.
     */
    void
    quit();

    /*!
     * You can reimplement this method to create your own custom main loop.
     */
    virtual void
    exec();

    /*!
     * Sets a background image.
     */
    void
    setBackgroundImage(const std::string& imagePath);

    /*!
     * Set application window layout.
     */
    void
    setLayout(LayoutBase* layout);

    /*!
     * Sets application window's layout margins.
     */
    void
    setMargins(int top, int bottom, int left, int right);

    /*!
     * Sets application window's layout margins.
     */
    void
    setMargin(const Margin& margin);

    /*!
     * Adds a toolbar to application.
     *
     * @param bar
     * @param positionNorth if true places toolbar at top, else at bottom.
     */
    bool
    setToolbar(ToolBar* toolbar, bool positionNorth = true);

    /*!
     * Post a custom user event.
     *
     * @param type of event [0-999].
     * @param data if not NULL, a pointer to your data.
     *
     * \warning make sure your event type is lower than 1000.
     */
    void
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
     * This signal is emitted after application window is painted and visible.
     */
    sigc::signal<void> sigVisible;

    /*!
     * This signal is emitted before application becomes hidden.
     */
    sigc::signal<void> sigHidden;

    /*!
     * This signal is emitted before application is terminated.
     */
    sigc::signal<void> sigQuit;

protected:
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

    AppWindow*
    appWindow() const;

    /*!
     * Shows application window.
     */
    void
    show();

    /*!
     * Hides application window.
     */
    void
    hide();

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

    /*!
     * Sets the stylist for application.
     *
     * \warning This method is not implemented.
     */
    static void
    setStylist(StylistBase* stylist);

    /*!
     * Sets the font pack for application.
     *
     * @param fontPack path to a valid font pack file.
     */
    static bool
    setFontPack(const char* fontPack);

    /*!
     * Sets the icon pack for application.
     *
     * @param iconPack path to a valid icon pack file
     */
    static bool
    setIconPack(const char* iconPack);

    /*!
     * Sets the palette file for stylist.
     */
    static bool
    setPaletteFromFile(const char* palette);

    /*!
     * Sets the style file for stylist.
     */
    static bool
    setStyleFromFile(const char* style);

    /*!
     * This method is called if custom compose is set.
     *
     * \sa setCustomCompose
     */
    virtual void
    compose(const PaintEvent& event);

private:
    //! AppBase instance.
    static Application* __instance;
    //! Event buffer for application.
    static IDirectFBEventBuffer* __buffer;

    AppWindow* _appWindow;
    //! Application state.
    AppFlags __flags;
    //! Old and new cursor positions.
    DFBPoint __cursorOld, __cursorNew;
    //! Max. available screen dimension.
    DFBDimension __layerSize;

    typedef std::list<Callback*> CallbackList;
    //! List of callbacks
    CallbackList __callbacks;
    //! Serialises access to __callbacks.
    pthread_mutex_t __cbMutex;

    typedef std::list<Timer*> TimerList;
    TimerList _timers;
    pthread_mutex_t __timerMutex;

    typedef std::list<WindowWidget*> WindowList;
    //! Application wide list of windows.
    WindowList __windowList;
    //! Window with focus.
    WindowWidget* __activeWindow;
    //! Serialises access to window list.
    pthread_mutex_t __windowMutex;

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
     * Returns current cursor position.
     */
    static DFBPoint
    cursorPosition();

    void
    initEventBuffer();

    void
    releaseEventBuffer();

    void
    handleKeyInputEvent(const DFBInputEvent& event, DFBWindowEventType type);

    void
    handleButtonInputEvent(const DFBInputEvent& event, DFBWindowEventType type);

    void
    handleButtonRelease(const DFBInputEvent& event);

    void
    handleAxisMotion(const DFBInputEvent& event);

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
    typedef std::list<SurfaceEventListener*> SurfaceListenerList;
    //! List of surface event listeners.
    SurfaceListenerList __selList;
    //! Serialises access to _selList.
    pthread_mutex_t __selMutex;

    // Surface event sync. stuff

    Timer* _update_timer;

    DFBSurfaceID _updateID;

    bool _update;

    bool _updateFromSurfaceView;

    bool _syncWithSurfaceEvents;

    unsigned int _updateFlipCount;

    long long _updateDiff;

    long long _updateTime;

    long long _updateDisable;

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

    void
    accountSurfaceEvent(const DFBSurfaceEvent& event, long long lastTime);

    void
    updateTimeout();

    void
    updateFromWindow();

    void
    disableSurfaceEventSync(long long micros);

#endif

    friend class AppWindow;
    friend class Callback;              // add/remove callback
    friend class ILXCompositor;
    friend class PlatformManager;
    friend class SurfaceEventListener;  // add/remove SurfaceEventListener
    friend class SurfaceView;
    friend class Timer;                 // add/remove timer
    friend class Window;                // appsize() and activeWindow()
    friend class WindowWidget;
};
}
#endif /* ILIXI_APPLICATION_H_ */
