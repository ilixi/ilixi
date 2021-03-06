/*
 Copyright 2010-2015 Tarik Sekmen.

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
#include <ui/AppWindow.h>

namespace ilixi
{

//! An application with a main window.
/*!
 * This class is used to create a new GUI application. You can always add and remove widgets or other windows to your application. You can access main window using appWindow() method.
 *
 * Main responsibilities of an application are to handle input events and maintain child windows. In case there are no modal windows, a window stack is used to forward events to the window on top.
 * In order to push custom key and pointer events you should use postKeyEvent and postPointerEvent respectively.
 *
 * Application uses Engine internally to control main loop (You can have only one application running at one time per process).
 * Once an application object is constructed, use exec() to enter main loop and call quit() or press ESC key to terminate application.
 * Main loop will wait unless there are callbacks, timers or input events. It will automatically wake up if there are pending input events.
 *
 * If you just need to perform tasks around other DirectFB applications without a GUI use Service instead.
 */
class Application : public sigc::trackable
{
public:
    /*!
     * Constructor.
     *
     * Make sure you pass command line parameters to constructor as it initializes platform interfaces.
     */
    Application(int* argc, char*** argv, AppOptions opts = OptNone);

    /*!
     * Destructor.
     */
    virtual
    ~Application();

    /*!
     * Returns a pointer to running application.
     */
    static Application*
    instance();

    /*!
     * Returns main window width in pixels.
     */
    int
    width() const;

    /*!
     * Returns main window height in pixels.
     */
    int
    height() const;

    /*!
     * Adds a new child widget to main window.
     *
     * \sa ContainerBase::addWidget()
     */
    bool
    addWidget(Widget* widget);

    /*!
     * Removes a child widget from main window.
     *
     * \sa addWidget()
     */
    bool
    removeWidget(Widget* widget);

    /*!
     * Updates main window.
     */
    void
    update();

    /*!
     * Terminates application.
     */
    static void
    quit();

    /*!
     * Enters main loop and waits for incoming events to process.
     *
     * \note You can reimplement this method to create your own custom main loop.
     */
    virtual void
    exec();

    /*!
     * Sets a background image.
     */
    void
    setBackgroundImage(const std::string& imagePath, bool tile = false);

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

#if ILIXI_HAS_GETFRAMETIME
    static long long
    getFrameTime();

    static void
    setFrameTime(long long micros);
#endif

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
        APS_NONE = 0x0000000,           //!< Default application state.
        APS_INITIALISED = 0x0000001,    //!< Application is initialised.
        APS_VISIBLE = 0x0000002,        //!< Application has a visible window and has access to events.
        APS_HIDDEN = 0x0000004,         //!< Application has no window and has no access to events.
        APS_CUSTOM = 0x0000008          //!< Disable waking up of buffer when an update is received.
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
     * This method is called if custom compose is set for main window.
     *
     * \sa AppWindow::setCustomCompose
     */
    virtual void
    compose(const PaintEvent& event);

private:
    //! AppBase instance.
    static Application* __instance;

    //! Dragging flag.
    bool _dragging;

    AppWindow* _appWindow;
    //! Application state.
    AppFlags __flags;
    //! Old and new cursor positions.
    DFBPoint __cursorOld, __cursorNew;
    //! Max. available screen dimension.
    DFBDimension __screenSize;

    typedef std::list<WindowWidget*> WindowList;
    //! Application wide list of windows.
    WindowList __windowList;
    //! Window with focus.
    WindowWidget* __activeWindow;
    //! Serialises access to window list.
    pthread_mutex_t __windowMutex;

    //! Frame time set during window update
    long long _frameTime;

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
    handleWindowEvents(const DFBWindowEvent& event);

    static void
    handleDragEvents(const DFBWindowEvent& event);

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

    static void
    setDragging(bool dragging);

    friend class AppWindow;
    friend class DragHelper;            // set _dragging
    friend class ILXCompositor;
    friend class PlatformManager;
    friend class SurfaceEventListener;  // accountSurfaceEvent
    friend class SurfaceView;
    friend class Window;                // appsize() and activeWindow()
    friend class WindowWidget;
};
}
#endif /* ILIXI_APPLICATION_H_ */
