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

namespace ilixi
{
  struct CompositedAppRecord
  {
    unsigned long appID; // Package manager ID
    unsigned int instanceID;
    pid_t processID;
    AppView* view;
    AppThumbnail* thumb;
    // TODO should add lookUpAppRecord(DFBWindowID)
    std::vector<DFBWindowID> _windows;
    pthread_mutex_t mutex;
  };

  class Compositor : public Application
  {
    friend void
    window_add(void *context, const DFBWindowInfo *info);

    friend void
    window_remove(void *context, DFBWindowID id);

    friend void
    window_config(void *context, DFBWindowID id, const DFBWindowConfig *config,
        DFBWindowConfigFlags flags);

    friend void
    window_state(void *context, DFBWindowID id, const DFBWindowState *state);

    friend void
    window_restack(void *context, DFBWindowID id, unsigned int index);

    friend void
    window_focus(void *context, DFBWindowID id);

  public:
    Compositor(int argc, char* argv[]);

    virtual
    ~Compositor();

    CompositedAppRecord*
    lookUpAppRecord(unsigned long appID);

    CompositedAppRecord*
    lookUpAppRecord(unsigned int instanceID);

    CompositedAppRecord*
    lookUpAppRecordUsingWindowID(DFBWindowID windowID);

    void
    addAppRecord(CompositedAppRecord* appRecord);

    void
    removeAppRecord(unsigned long appID);

    IDirectFBWindow*
    getWindow(DFBWindowID id);

    void
    focusWindow(IDirectFBWindow* window);

    void
    showLauncher(bool show);

    void
    showSwitcher(bool show);

    void
    handleViewRequest(unsigned long appID);

    void
    handleSwitchRequest();

    void
    handleQuit();

  private:
    CompositedAppRecord* _currentApp;

    typedef std::vector<CompositedAppRecord*> AppVector;
    AppVector _appVector;

    IDirectFBWindows* _iWindows;
    DFBWindowsWatcher _watcher;

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
      CET_State //!< Window state
    };

    struct CompositorEvent
    {
      DFBEventClass clazz;
      CompositorEventType type;
    };

    struct CompositorEventData
    {
      CompositorEventData() :
          windowID(0), appRecord(0), config(0), flags(DWCONF_NONE)
      {
      }

      DFBWindowID windowID;
      CompositedAppRecord* appRecord;
      const DFBWindowConfig* config;
      DFBWindowConfigFlags flags;
    };

    void
    onVisible();

    void
    addWindow(const DFBWindowInfo* info);

    void
    removeWindow(DFBWindowID id);

    void
    configWindow(DFBWindowID window_id, const DFBWindowConfig *config,
        DFBWindowConfigFlags flags);

    void
    stateWindow(DFBWindowID id, const DFBWindowState* state);

    void
    restackWindow(DFBWindowID id, unsigned int index);

    void
    focusWindow(DFBWindowID id);

    void
    updateCompositorGeometry();

    virtual void
    handleUserEvent(const DFBUserEvent& event);

    virtual bool
    windowPreEventFilter(const DFBWindowEvent& event);

    virtual void
    compose(const Rectangle& rect);
  };

} /* namespace ilixi */
#endif /* COMPOSITOR_H_ */
