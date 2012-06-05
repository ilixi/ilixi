/*
 Copyright 2012 Tarik Sekmen.

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
#include "ui/Application.h"
#include "Switcher.h"
#include "Launcher.h"
#include "HomeButton.h"
#include "SwitchButton.h"
#include "QuitButton.h"
#include "AppView.h"
#include "ApplicationManager.h"

namespace ilixi
{

  class Compositor : public Application
  {
    friend class ApplicationManager;

  public:
    Compositor(int argc, char* argv[]);

    virtual
    ~Compositor();

    ApplicationManager*
    appMan() const;

    void
    showLauncher(bool show);

    void
    showSwitcher(bool show);

    void
    handleViewRequest(AppInstance* instance);

    void
    handleSwitchRequest();

    void
    handleQuit();

  private:
    ApplicationManager* _appMan;
    AppInstance* _currentApp;

    Switcher* _switcher;
    Launcher* _launcher;
    HomeButton* _homeButton;
    SwitchButton* _switchButton;
    QuitButton* _quitButton;

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
      CompositorEventData() :
          instance(NULL), windowID(0), reconfig(0)
      {
      }

      AppInstance* instance;
      DFBWindowID windowID;
      SaWManWindowReconfig* reconfig;
    };

    void
    onVisible();

    IDirectFBWindow*
    getWindow(DFBWindowID id);

    void
    addWindow(AppInstance* instance, const SaWManWindowInfo* info);

    void
    removeWindow(AppInstance* instance, const SaWManWindowInfo* info);

    void
    configWindow(AppInstance* instance, SaWManWindowReconfig *reconfig,
        SaWManWindowInfo* winInfo);

    void
    stateWindow(DFBWindowID id, const DFBWindowState* state);

    void
    restackWindow(DFBWindowID id, unsigned int index);

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
