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
#include "CompositorSurfaceView.h"
#include "HomeButton.h"
#include "SwitchButton.h"

namespace ilixi
{

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

    void
    next();

    void
    previous();

  protected:
    void
    addApps();

    IDirectFBWindow*
    getWindow(DFBWindowID id);

    void
    focusWindow(IDirectFBWindow* window);

    void
    animLauncher(bool hide = false);

    void
    animSwitcher(bool hide = false);

    void
    handleViewRequest(CompositorSurfaceView* csw);

    void
    handleSwitchRequest();

  private:
    typedef std::vector<CompositorSurfaceView*> Surfaces;
    Surfaces _surfaces;
    CompositorSurfaceView* _currentSurface;
    CompositorSurfaceView* _preSurface;

    bool _launcherOn;

    IDirectFBWindows* _iWindows;
    DFBWindowsWatcher _watcher;

    Switcher* _switcher;
    Launcher* _launcher;
    HomeButton* _homeButton;
    SwitchButton* _switchButton;

    Font* _switcherFont;
    Font* _overlayFont;

    enum CompositorEventType
    {
      CET_Add, CET_Remove, CET_Config, CET_Focus, CET_Restack, CET_State
    };

    struct CompositorEvent
    {
      DFBEventClass clazz;
      CompositorEventType type;
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

    CompositorSurfaceView*
    lookUpSurfaceView(DFBSurfaceID id);

    void
    updateCompositorGeometry();

    virtual void
    handleUserEvent(const DFBUserEvent& event);

    virtual bool
    windowPreEventFilter(const DFBWindowEvent& event);

    virtual bool
    windowPostEventFilter(const DFBWindowEvent& event);

  };

} /* namespace ilixi */
#endif /* COMPOSITOR_H_ */
