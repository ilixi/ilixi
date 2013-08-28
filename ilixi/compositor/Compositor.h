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

#ifndef ILIXI_COMPOSITOR_H_
#define ILIXI_COMPOSITOR_H_

#include <compositor/ApplicationManager.h>
#include <compositor/AppView.h>
#include <compositor/CompositorComponent.h>
#include <compositor/OSKComponent.h>
#include <compositor/SoundComponent.h>
#include <compositor/Switcher.h>
#include <lib/FPSCalculator.h>
#include <core/Application.h>

extern "C"
{
#include <directfb_windows.h>
}

namespace ilixi
{
class Label;
//! Base class for compositor applications.
class ILXCompositor : public Application
{
    friend class ApplicationManager;
    friend class CompositorComponent;
    friend class NotificationManager;
    friend class OSKComponent;
    friend class AppView;
    friend class Notification;

public:
    /*!
     * Constructor.
     *
     * Creates ApplicationManager and Coma components.
     */
    ILXCompositor(int argc, char* argv[]);

    /*!
     * Destructor.
     *
     * Deletes all components.
     */
    virtual
    ~ILXCompositor();

    /*!
     * Sets a new switcher and removes old one.
     */
    void
    setSwitcher(Switcher* switcher);

    /*!
     * Returns the application manager instance.
     */
    ApplicationManager*
    appMan() const;

    /*!
     * Hides current application and makes given instance visible.
     */
    void
    showInstance(AppInstance* instance);

    /*!
     * Shows launcher screen and hides current application.
     *
     * @param show launcher if true, hide otherwise.
     */
    void
    toggleLauncher(bool show);

    /*!
     * Shows application switcher.
     *
     * @param show switcher if true, hide otherwise.
     */
    void
    toggleSwitcher(bool show);

    /*!
     * Hides or shows OSK.
     */
    void
    toggleOSK(bool show);

    /*!
     * Terminates client application.
     */
    void
    killApp(AppInstance* instance);

    const Rectangle&
    getAppGeometry() const;

    const Rectangle&
    getBarGeometry() const;

    const Rectangle&
    getOskGeometry() const;

    const Rectangle&
    getSwitcherGeometry() const;

protected:
    //! Launcher instance.
    AppInstance* _home;
    //! StatusBar instance.
    AppInstance* _statusBar;
    //! Current application instance.
    AppInstance* _currentApp;

    //! Switcher instance.
    Switcher* _switcher;

    /*!
     * Shows OSK and centers given rect at top.
     */
    void
    showOSK(DFBRectangle rect, pid_t process);

    /*!
     * Sends given key to current application instance.
     */
    void
    sendOSKInput(uint32_t key, unsigned int mask = 0);

    void
    setLayerOpacity(u8 opacity);

    /*!
     * Just updates FPS.
     */
    void
    compose(const PaintEvent& event);

    /*!
     * Sets on-screen-keyboard geometry.
     */
    void
    setOSKGeometry(const Rectangle& rect);

    /*!
     * Sets foreground application geometry.
     */
    void
    setAppGeometry(const Rectangle& rect);

    /*!
     * Sets statusbar geometry.
     */
    void
    setBarGeometry(const Rectangle& rect);

    /*!
     * Sets switcher geometry.
     */
    void
    setSwitcherGeometry(const Rectangle& rect);

private:
    //! Application manager instance.
    ApplicationManager* _appMan;

    //! Previous application instance.
    AppInstance* _previousApp;

    //! FPS calculator refreshed with each paintEvent.
    FPSCalculator* _fps;

    //! CompositorComponent instance.
    CompositorComponent* _compComp;
    //! SoundComponent instance.
    SoundComponent* _soundComp;
    //! OSKComponent instance.
    OSKComponent* _oskComp;

    //! OSK instance.
    AppInstance* _osk;

    //! PID of application which requested OSK input.
    pid_t _oskTargetPID;
    //! Bounding rectangle around current OSK input target.
    Rectangle _oskTargetRect;

    //! This enum specifies a few window compositing related events.
    enum CompositorEventType
    {
        CET_Add,        //!< Window added
        CET_Remove,     //!< Window removed
        CET_Config,     //!< Window configured
        CET_Focus,      //!< Window focused
        CET_Restack,    //!< Window restack
        CET_State,      //!< Window state
        CET_Quit,       //!< Application terminated
        CET_Term,       //!< Terminate application.
        CET_Crash       //!< Application crashed.
    };

    //! This struct specifies a CompositorEvent.
    /*!
     * These events are pushed to compositor application's event buffer.
     */
    struct CompositorEvent
    {
        DFBEventClass clazz;
        CompositorEventType type;
    };

    //! This struct is used to carry data with compositor events.
    struct CompositorEventData
    {
        CompositorEventData()
                : instance(NULL),
                  windowID(0),
                  reconfig(0)
        {
        }

        AppInstance* instance;
        DFBWindowID windowID;
        SaWManWindowReconfig* reconfig;
    };

    //! This struct is used to store settings for compositor and its components.
    struct CompositorSettings
    {
        CompositorSettings()
                : clickFocus(true),
                  animations(true),
                  hideAnimProps(AppView::None),
                  showAnimProps(AppView::None),
                  durationSlide(300),
                  durationShow(300),
                  durationHide(500),
                  filter(),
                  notificationTimeout(5000),
                  memMonitor(true),
                  memCritical(0.2),
                  memLow(0.5),
                  pgCritical(30),
                  pgLow(10)
        {
        }

        bool clickFocus;                            //!< Click to focus application in foreground.
        bool animations;
        AppView::AnimatedProperty hideAnimProps;    //!< Animated properties for hiding application.
        AppView::AnimatedProperty showAnimProps;    //!< Animated properties for visible application.
        unsigned int durationSlide;
        unsigned int durationShow;                  //!< Animation duration for show.
        unsigned int durationHide;                  //!< Animation duration for hide.
        DFBConvolutionFilter filter;                //!< Convolution filter.
        unsigned int notificationTimeout;           //!< Notification will hide itself after timeout(ms).
        bool memMonitor;
        double memCritical;
        double memLow;
        int pgCritical;
        int pgLow;
    };

    //! This property is used by compositor components.
    CompositorSettings settings;

    Rectangle _oskGeometry;
    Rectangle _appGeometry;
    Rectangle _barGeometry;
    Rectangle _switcherGeometry;

    //! Starts auto-start applications using application manager.
    void
    onVisible();

    //! Returns a window on ui layer given a DFBWindowID if found, else return NULL.
    IDirectFBWindow*
    getWindow(DFBWindowID id);

    void
    addWindow(AppInstance* instance, const SaWManWindowInfo* info);

    void
    removeWindow(AppInstance* instance, const SaWManWindowInfo* info);

    void
    configWindow(AppInstance* instance, SaWManWindowReconfig *reconfig, const SaWManWindowInfo* info);

    void
    restackWindow(AppInstance* instance, const SaWManWindowInfo* info, int order, DFBWindowID other);

    void
    stateWindow(DFBWindowID id, const DFBWindowState* state);

    void
    focusWindow(DFBWindowID id);

    void
    processRemoved(AppInstance* instance);

    void
    processTerminated(AppInstance* instance);

    virtual void
    handleUserEvent(const DFBUserEvent& event);

    virtual bool
    windowPreEventFilter(const DFBWindowEvent& event);

    virtual bool
    windowCustomEventFilter(const DFBWindowEvent& event);

    bool
    parseSettings();

    void
    appVisible();
};

} /* namespace ilixi */
#endif /* ILIXI_COMPOSITOR_H_ */
