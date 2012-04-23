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

//#define DIRECT_ENABLE_DEBUG
#include "Compositor.h"
#include "core/Logger.h"
#include "sigc++/bind.h"

#define HS_H 196

using namespace std;

namespace ilixi
{
  D_DEBUG_DOMAIN( ILX_COMPOSITOR, "ilixi/compositor", "Compositor");

  //*************************************************************************
  void
  window_add(void *context, const DFBWindowInfo *info)
  {
    ((Compositor*) context)->addWindow(info);
  }

  void
  window_remove(void *context, DFBWindowID id)
  {
    ((Compositor*) context)->removeWindow(id);
  }

  void
  window_config(void *context, DFBWindowID id, const DFBWindowConfig *config,
      DFBWindowConfigFlags flags)
  {
    ((Compositor*) context)->configWindow(id, config, flags);
  }

  void
  window_state(void *context, DFBWindowID id, const DFBWindowState *state)
  {
    ((Compositor*) context)->stateWindow(id, state);
  }

  void
  window_restack(void *context, DFBWindowID id, unsigned int index)
  {
    ((Compositor*) context)->restackWindow(id, index);
  }

  void
  window_focus(void *context, DFBWindowID id)
  {
    ((Compositor*) context)->focusWindow(id);
  }

  //*************************************************************************

  Compositor::Compositor(int argc, char* argv[]) :
      Application(argc, argv, OptCompositor), _currentSurface(NULL), _preSurface(
          NULL), _launcherOn(true)
  {
    setTitle("Compositor");
    setBackgroundImage(ILIXI_DATADIR"images/ilixi_bg.jpg");

    getDFB()->GetInterface(getDFB(), "IDirectFBWindows", NULL, NULL,
        (void**) &_iWindows);

    setMargin(0);

    _watcher.WindowAdd = window_add;
    _watcher.WindowConfig = window_config;
    _watcher.WindowFocus = window_focus;
    _watcher.WindowRemove = window_remove;
    _watcher.WindowRestack = window_restack;
    _watcher.WindowState = window_state;

    _iWindows->RegisterWatcher(_iWindows, &_watcher, this);

    _switcherFont = new Font(ILIXI_DATADIR"fonts/decker.ttf");
    _switcherFont->setSize(24);

    _overlayFont = new Font(ILIXI_DATADIR"fonts/decker.ttf");
    _overlayFont->setSize(72);

    _launcher = new Launcher();
    addWidget(_launcher);
    _launcher->sigViewRequest.connect(
        sigc::mem_fun(this, &Compositor::handleViewRequest));

    _switcher = new Switcher();
    _switcher->sigSwitchRequest.connect(
        sigc::mem_fun(this, &Compositor::handleSwitchRequest));
    addWidget(_switcher);
    addApps();

    _homeButton = new HomeButton();
    _homeButton->sigPressed.connect(
        sigc::bind<bool>(sigc::mem_fun(this, &Compositor::animLauncher),
            false));
    addWidget(_homeButton);

    _switchButton = new SwitchButton();
    _switchButton->setVisible(false);
    _switchButton->sigPressed.connect(
        sigc::bind<bool>(sigc::mem_fun(this, &Compositor::animSwitcher),
            false));
    addWidget(_switchButton);

    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &Compositor::updateCompositorGeometry));
    sigVisible.connect(sigc::mem_fun(this, &Compositor::onVisible));
  }

  Compositor::~Compositor()
  {
    delete _switcherFont;
    delete _overlayFont;
    _iWindows->Release(_iWindows);
  }

  void
  Compositor::next()
  {
    _switcher->scrollTo(_switcher->getNextCW());
  }

  void
  Compositor::previous()
  {
    _switcher->scrollTo(_switcher->getPreviousCW());
  }

  void
  Compositor::addApps()
  {
    // dfb-examples
    _launcher->addApplication("Penguins", "df_andi", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/andi.png");

    _launcher->addApplication("Neo", "df_neo", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/neo.png");

    _launcher->addApplication("Inputs", "df_input", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/input.png");

    _launcher->addApplication("Matrix", "df_matrix", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/matrix.png");

    _launcher->addApplication("Knuckles", "df_knuckles", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/knuckles.png");

    _launcher->addApplication("Particle", "df_particle", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/particle.png");

    _launcher->addApplication("Texture", "df_texture", "--dfb:force-windowed",
        ILIXI_DATADIR"compositor/texture.png");

    // lite
    _launcher->addApplication("DFBTerm", "dfbterm", NULL,
        ILIXI_DATADIR"compositor/dfbterm.png");

    // ilixi
    _launcher->addApplication("Gallery", "ilixi_demo1", NULL,
        ILIXI_DATADIR"compositor/gallery.png");

    // others
    _launcher->addApplication("Video Player", "dfbtest_video",
        ILIXI_DATADIR"compositor/demo.mp4 -f RGB32 -l --dfb:force-windowed",
        ILIXI_DATADIR"compositor/player.png");

    _launcher->addApplication("ClanBomber 2", "clanbomber2",
        "--dfb:force-windowed", ILIXI_DATADIR"compositor/clanbomber2.png");

    // Check if installed
    _launcher->addApplication("QML Viewer", "qmlviewer",
        "-qws -display directfb", ILIXI_DATADIR"compositor/qt-qml.png");

    _launcher->addApplication("WebKitDFB", "WebKitDFB", NULL,
        ILIXI_DATADIR"compositor/webkitdfb.png");
  }

  IDirectFBWindow*
  Compositor::getWindow(DFBWindowID id)
  {
    if (id)
      {
        IDirectFBDisplayLayer* layer;
        DFBResult ret = getDFB()->GetDisplayLayer(getDFB(), DLID_PRIMARY,
            &layer);
        if (ret)
          {
            ILOG_ERROR(
                ILX_COMPOSITOR, "Error! GetDisplayLayer: %s", DirectFBErrorString(ret));
            return NULL;
          }

        IDirectFBWindow* window;
        ret = layer->GetWindow(layer, id, &window);
        if (ret)
          {
            ILOG_ERROR(
                ILX_COMPOSITOR, "Error! GetWindow: %s", DirectFBErrorString(ret));
            return NULL;
          }
        return window;
      }
    return NULL;
  }

  void
  Compositor::focusWindow(IDirectFBWindow* window)
  {
    static IDirectFBWindow* preWindow = NULL;
    static IDirectFBWindow* currentWindow = NULL;

    if (preWindow && preWindow != window)
      {
        DFBWindowEvent event;
        event.flags = DWEF_NONE;
        event.type = DWET_LOSTFOCUS;
        preWindow->SendEvent(preWindow, &event);
      }

    if (window != currentWindow)
      {
        DFBWindowEvent event;
        event.flags = DWEF_NONE;
        event.type = DWET_GOTFOCUS;
        currentWindow = preWindow = window;
        window->SendEvent(window, &event);
      }

  }

  void
  Compositor::animLauncher(bool hide)
  {
    if (hide)
      {
        if (_currentSurface)
          {
            _launcherOn = false;
            _launcher->setVisible(_launcherOn);
            _currentSurface->show();
            _homeButton->show();

            if (_surfaces.size() > 1)
              _switchButton->show();

            focusWindow(_currentSurface->_window);
          }
      }
    else
      {
        if (_currentSurface)
          {
            _launcherOn = true;
            _launcher->setVisible(_launcherOn);
            _currentSurface->hide();
            _homeButton->hide();
            focusWindow (activeDFBWindow());
          }
        animSwitcher(true);
      }
  }

  void
  Compositor::animSwitcher(bool hide)
  {
    if (_surfaces.size() < 2)
      return;

    if (hide)
      {
        _switcher->hide();
        _switchButton->show();
      }
    else
      {
        _switcher->show();
        _switchButton->hide();
      }
  }

  void
  Compositor::handleViewRequest(CompositorSurfaceView* csw)
  {
    if (csw)
      {
        if (_currentSurface)
          _currentSurface->hide();
        _currentSurface = csw;
        _launcherOn = false;
        _launcher->setVisible(_launcherOn);
        _currentSurface->show();
        _homeButton->show();

        if (_surfaces.size() > 1)
          {
            _switchButton->show();
            if (_switcher->visible())
              _switcher->hide();
          }
        focusWindow(_currentSurface->_window);
      }
  }

  void
  Compositor::handleSwitchRequest()
  {
    if (_switcher->visible())
      animSwitcher(true);

    if (_switcher->getCurrentCW()->sourceID() != _currentSurface->sourceID())
      {
        _currentSurface->hide();
        _currentSurface = lookUpSurfaceView(
            _switcher->getCurrentCW()->sourceID());

        if (_launcherOn)
          {
            _launcherOn = false;
            _launcher->setVisible(_launcherOn);
          }
        _currentSurface->show();
        _homeButton->show();
        focusWindow(_currentSurface->_window);
      }
  }

  void
  Compositor::onVisible()
  {
    activeDFBWindow()->GrabKey(activeDFBWindow(), DIKS_TAB, DIMM_ALT);
  }

  void
  Compositor::addWindow(const DFBWindowInfo* info)
  {
    if (info->window_id > 1)
      {
        ILOG_DEBUG(
            ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, info->window_id);
        ILOG_DEBUG(ILX_COMPOSITOR, "  -> caps         0x%08x\n", info->caps);
        ILOG_DEBUG(
            ILX_COMPOSITOR, "  -> res. id  0x%016llx\n", (unsigned long long) info->resource_id);

        IDirectFBWindow* dfbWindow = getWindow(info->window_id);
        dfbWindow->MoveTo(dfbWindow, width(), 0);

        postUserEvent(CET_Add, dfbWindow);
      }
  }

  void
  Compositor::removeWindow(DFBWindowID id)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, id);
    // TODO remove window
    //_switcher->removeCW(_cwindows[id]);
    update();
  }

  void
  Compositor::configWindow(DFBWindowID id, const DFBWindowConfig *config,
      DFBWindowConfigFlags flags)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, id);
    ILOG_DEBUG(ILX_COMPOSITOR, "  -> flags        0x%08x\n", flags);
  }

  void
  Compositor::stateWindow(DFBWindowID id, const DFBWindowState* state)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, id);
    ILOG_DEBUG(ILX_COMPOSITOR, "  -> flags        0x%08x\n", state->flags);
  }

  void
  Compositor::restackWindow(DFBWindowID id, unsigned int index)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, id);
    ILOG_DEBUG(ILX_COMPOSITOR, "  -> index        %u\n", index);
  }

  void
  Compositor::focusWindow(DFBWindowID id)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, id);
  }

  CompositorSurfaceView*
  Compositor::lookUpSurfaceView(DFBSurfaceID id)
  {
    for (unsigned int i = 0; i < _surfaces.size(); ++i)
      if (_surfaces[i]->sourceID() == id)
        return _surfaces[i];
    return NULL;
  }

  void
  Compositor::updateCompositorGeometry()
  {
    _launcher->setGeometry(0, 0, width(), height());
    _switcher->setGeometry(0, height() - HS_H, width(), HS_H);
    _homeButton->moveTo(0, 0);
    _switchButton->moveTo(0, height() - 80);
  }

  void
  Compositor::handleUserEvent(const DFBUserEvent& event)
  {
    if (event.clazz == DFEC_USER)
      {
        IDirectFBWindow* dfbWindow = (IDirectFBWindow*) event.data;
        switch (event.type)
          {
        case CET_Add:
          {

            WindowThumbnail* thumb = new WindowThumbnail(
                _launcher->getCurrent()->text());

            CompositorSurfaceView* surface = new CompositorSurfaceView();
            addWidget(surface);
            widgetToBack(surface);
            _currentSurface = surface;

            thumb->setSourceFromWindow(dfbWindow);
            surface->setSourceFromWindow(dfbWindow);
            surface->setGeometry(0, 0, width(), height());
            surface->_window = dfbWindow;

            _launcher->getCurrent()->setThumb(thumb);
            _launcher->getCurrent()->setCompositedSurface(surface);
            _switcher->addCW(thumb);
            _surfaces.push_back(surface);

            animLauncher(true);
          }
          break;

        case CET_Remove:
        case CET_Config:
        case CET_Focus:
        case CET_Restack:
        case CET_State:
          break;

        default:
          break;
          }
      }
  }

  bool
  Compositor::windowPreEventFilter(const DFBWindowEvent& event)
  {
    switch (event.type)
      {
    case DWET_KEYDOWN:
      if (_currentSurface
          && ((event.key_symbol == DIKS_SMALL_H)
              || (event.key_symbol == DIKS_HOME)))
        {
          if (_launcherOn)
            animLauncher(true);
          else
            animLauncher(false); // show launcher
          return true;
        }
      else if (event.key_symbol == DIKS_TAB && event.modifiers == DIMM_ALT)
        {
          if (_surfaces.size() < 2)
            return true;

          if (!_switcher->visible())
            animSwitcher(false); // show switcher
          next();
          return true;
        }
      break;

    case DWET_KEYUP:
      {
        if (_currentSurface && event.key_symbol == DIKS_ALT)
          {
            if (_surfaces.size() < 2)
              return true;

            if (_switcher->visible())
              animSwitcher(true);

            if (_launcherOn)
              {
                _launcherOn = false;
                _launcher->setVisible(_launcherOn);
              }

            if (_switcher->getCurrentCW()->sourceID()
                != _currentSurface->sourceID())
              {
                _currentSurface->hide();
                _currentSurface = lookUpSurfaceView(
                    _switcher->getCurrentCW()->sourceID());
                _launcherOn = false;
                _currentSurface->show();
                focusWindow(_currentSurface->_window);
              }
            return true;
          }
      }
      break;

    default:
      break;
      }
    return false;
  }

  bool
  Compositor::windowPostEventFilter(const DFBWindowEvent& event)
  {
    if (!_launcherOn)
      return _currentSurface->consumeWindowEvent(event);
    return false;
  }

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
  ilixi::Compositor app(argc, argv);
  app.exec();
  return 0;
}
