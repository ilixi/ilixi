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
#include "Launcher.h"
#include "QuitButton.h"
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
class Compositor : public Application
{
    friend class ApplicationManager;
    friend class CompositorComponent;
    friend class NotificationManager;
    friend class OSKComponent;

public:
    /*!
     * Constructor.
     *
     * Creates ApplicationManager and Coma components.
     */
    Compositor(int argc, char* argv[]);

    /*!
     * Destructor.
     *
     * Deletes all components.
     */
    virtual
    ~Compositor();

    /*!
     * Returns the application manager instance.
     */
    ApplicationManager*
    appMan() const;

    /*!
     * Shows launcher screen and hides current application.
     *
     * @param show launcher if true, hide otherwise.
     */
    void
    showLauncher(bool show);

    /*!
     * Shows application switcher.
     *
     * @param show switcher if true, hide otherwise.
     */
    void
    showSwitcher(bool show);

    /*!
     * Shows given application instance and hides launcher.
     *
     * @param instance
     */
    void
    handleViewRequest(AppInstance* instance);

    /*!
     * Hides current application and shows
     * the selected application in switcher.
     */
    void
    handleSwitchRequest();

    /*!
     *Terminates client application or compositor.
     */
    void
    handleQuit();

protected:
    /*!
     * Adds a overlay on top.
     */
    void
    addOverlay(DFBSurfaceID id);

    /*!
     * Adds a dialog on top.
     */
    void
    addDialog(DFBSurfaceID id);

    /*!
     * Shows OSK and centers given rect at top.
     */
    void
    showOSK(DFBRectangle rect);

    /*!
     * Hides OSK.
     */
    void
    hideOSK();

    void
    sendOSKInput(uint32_t key);

    void
    compose(const PaintEvent& event);

private:
    //! Application manager instance.
    ApplicationManager* _appMan;
    //! Current application instance.
    AppInstance* _currentApp;

    //! Switcher instance.
    Switcher* _switcher;
    //! Launcher screen instance.
    Launcher* _launcher;

    //! Buttons
    QuitButton* _quitButton;

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
    AppInstance* _statusBar;
    AppInstance* _osk;

    enum CompositorEventType
    {
        CET_Add, //!< Window added
        CET_Remove, //!< Window removed
        CET_Config, //!< Window configured
        CET_Focus, //!< Window focused
        CET_Restack, //!< Window restack
        CET_State, //!< Window state
        CET_Quit //!< Application terminated
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
    configWindow(AppInstance* instance, SaWManWindowReconfig *reconfig,
                 const SaWManWindowInfo* info);

    void
    restackWindow(AppInstance* instance, const SaWManWindowInfo* info,
                  int order, DFBWindowID other);

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
