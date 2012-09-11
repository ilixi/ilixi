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

#ifndef COMPOSITOR_H_
#define COMPOSITOR_H_

#include <map>
#include <directfb_windows.h>
#include <lib/FPSCalculator.h>
#include <ui/Application.h>
#include "AppView.h"
#include "ApplicationManager.h"
#include "Switcher.h"
#include "component/CompositorComponent.h"
#include "component/SoundComponent.h"
#include "component/OSKComponent.h"

namespace ilixi
{
/*!
 * Main application.
 */
class ILXCompositor : public Application
{
    friend class ApplicationManager;
    friend class CompositorComponent;
    friend class NotificationManager;
    friend class OSKComponent;
    friend class AppView;

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
     * Returns the application manager instance.
     */
    ApplicationManager*
    appMan() const;

    /*!
     * Hide current application, if any, and makes given instance visible.
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

protected:
    /*!
     * Shows OSK and centers given rect at top.
     */
    void
    showOSK(DFBRectangle rect, pid_t process);

    void
    sendOSKInput(uint32_t key);

    void
    showSound(bool show);

    void
    showDash(bool show);

    void
    setLayerOpacity(u8 opacity);

    void
    compose(const PaintEvent& event);

private:
    //! Application manager instance.
    ApplicationManager* _appMan;
    //! Current application instance.
    AppInstance* _currentApp;
    AppInstance* _previousApp;

    //! Switcher instance.
    Switcher* _switcher;

    //! FPS stuff
    Label* _fpsLabel;
    FPSCalculator* _fps;

    //! CompositorComponent instance.
    CompositorComponent* _compComp;
    //! SoundComponent instance.
    SoundComponent* _soundComp;
    //! OSKComponent instance.
    OSKComponent* _oskComp;

    // OSK
    AppInstance* _home;
    AppInstance* _statusBar;
    AppInstance* _osk;
    AppInstance* _dash;
    AppInstance* _mixer;

    pid_t _oskTargetPID;

    Rectangle _oskTargetRect;

    enum CompositorEventType
    {
        CET_Add,        //!< Window added
        CET_Remove,     //!< Window removed
        CET_Config,     //!< Window configured
        CET_Focus,      //!< Window focused
        CET_Restack,    //!< Window restack
        CET_State,      //!< Window state
        CET_Quit,       //!< Application terminated
        CET_Term        //!< Terminate application.
    };

    struct CompositorEvent
    {
        DFBEventClass clazz;
        CompositorEventType type;
    };

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

    struct CompositorSettings
    {
        bool clickFocus;          //!< Click to focus application in foreground.
        bool animations;
        AppView::AnimatedProperty hideAnimProps; //!< Animated properties for hiding application.
        AppView::AnimatedProperty showAnimProps; //!< Animated properties for visible application.
        unsigned int durationShow;             //!< Animation duration for show.
        unsigned int durationHide;              //!< Animation duration for hide.
        DFBConvolutionFilter filter;                //!< Convolution filter.
    };

    static CompositorSettings settings;

    void
    onVisible();

    void
    onFPSUpdate(float fps);

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
    updateCompositorGeometry();

    virtual void
    handleUserEvent(const DFBUserEvent& event);

    virtual bool
    windowPreEventFilter(const DFBWindowEvent& event);
};

} /* namespace ilixi */
#endif /* COMPOSITOR_H_ */
