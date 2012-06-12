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

#include "Compositor.h"
#include "graphics/Painter.h"
#include "core/Logger.h"
#include "sigc++/bind.h"
#include "switcher/HorizontalSwitcher.h"
#include "switcher/CarouselSwitcher.h"

using namespace std;

namespace ilixi
{
  D_DEBUG_DOMAIN( ILX_COMPOSITOR, "ilixi/compositor", "Compositor");

  Compositor::Compositor(int argc, char* argv[]) :
      Application(&argc, &argv, OptExclusive), _appMan(NULL), _currentApp(NULL), _switcher(
          NULL), _launcher(NULL), _homeButton(NULL), _switchButton(NULL), _quitButton(
          NULL), _fpsLabel(NULL), _fps(NULL)
  {
    _appMan = new ApplicationManager(this);

    setTitle("Compositor");
    setBackgroundImage(ILIXI_DATADIR"images/ilixi_bg.jpg");
    setMargin(0);

    _launcher = new Launcher(this);
    addWidget(_launcher);

    for (int i = 1; i < argc; i++)
      {
        if (strcmp(argv[i], "carousel") == 0)
          _switcher = new CarouselSwitcher();

        else if (strcmp(argv[i], "fps") == 0)
          {
            _fpsLabel = new Label("FPS");
            _fpsLabel->layout().setAlignment(TextLayout::Center);
            addWidget(_fpsLabel);

            _fps = new FPSCalculator();
            _fps->sigUpdated.connect(
                sigc::mem_fun(this, &Compositor::onFPSUpdate));
            ILOG_INFO(ILX_APPBASE, "FPS\n");
          }
      }

    if (_switcher == NULL)
      _switcher = new HorizontalSwitcher();

    _switcher->sigSwitchRequest.connect(
        sigc::mem_fun(this, &Compositor::handleSwitchRequest));
    addWidget(_switcher);

    _homeButton = new HomeButton("comp_home.png");
    _homeButton->setZ(5);
    _homeButton->sigPressed.connect(
        sigc::bind<bool>(sigc::mem_fun(this, &Compositor::showLauncher), true));
    addWidget(_homeButton);

    _switchButton = new SwitchButton("comp_switch.png");
    _switchButton->setZ(5);
    _switchButton->setVisible(false);
    _switchButton->sigPressed.connect(
        sigc::bind<bool>(sigc::mem_fun(this, &Compositor::showSwitcher), true));
    addWidget(_switchButton);

    _quitButton = new QuitButton("comp_quit.png");
    _quitButton->setZ(5);
    _quitButton->sigPressed.connect(
        sigc::mem_fun(this, &Compositor::handleQuit));
    addWidget(_quitButton);

    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &Compositor::updateCompositorGeometry));
    sigVisible.connect(sigc::mem_fun(this, &Compositor::onVisible));
  }

  Compositor::~Compositor()
  {
    delete _fps;
    delete _appMan;
    ILOG_TRACE_W(ILX_COMPOSITOR);
  }

  ApplicationManager*
  Compositor::appMan() const
  {
    return _appMan;
  }

  void
  Compositor::showLauncher(bool show)
  {
    if (show && !_launcher->visible())
      {
        if (_currentApp)
          _currentApp->view()->hide();
        _backgroundFlags = BGFAll;
        _launcher->setVisible(true);
        _homeButton->hide();
        showSwitcher(false);
      }
    else
      {
        if (_currentApp)
          {
            _switcher->setNeighbour(Up, _currentApp->view());
            _currentApp->view()->show();
            AppInfo* info = _appMan->infoByAppID(_currentApp->appID());
            if (info->appFlags() & APP_NEEDS_CLEAR)
              _backgroundFlags = BGFAll;
            else
              _backgroundFlags = BGFNone;
          }
        else
          _backgroundFlags = BGFAll;

        _launcher->setVisible(false);
        _homeButton->show();
        showSwitcher(false);
      }
  }

  void
  Compositor::showSwitcher(bool show)
  {
    if (_appMan->instanceCount() < 2)
      return;

    if (show)
      {
        _switchButton->hide();
        _switcher->show();
        eventManager()->setGrabbedWidget(NULL);
      }
    else
      {
        _switcher->hide();
        _switchButton->show();
      }
  }

  void
  Compositor::handleViewRequest(AppInstance* instance)
  {
    _currentApp = instance;
    showLauncher(false);
  }

  void
  Compositor::handleSwitchRequest()
  {
    if (_currentApp)
      _currentApp->view()->hide();

    _currentApp = _switcher->currentThumb()->instance();
    showLauncher(false);

    if (_switcher->visible())
      showSwitcher(false);
  }

  void
  Compositor::handleQuit()
  {
    if (_launcher->visible())
      quit();
    else if (_currentApp)
      {
        removeWidget(_currentApp->view());
        _switcher->removeThumb(_currentApp->thumb());
        _appMan->stopApplication(_currentApp->pid());
        _currentApp = NULL;
        showLauncher(true);
      }
  }

  void
  Compositor::onVisible()
  {
    _quitButton->show();
    if (_fps)
      _fps->start();
  }

  void
  Compositor::onFPSUpdate(float fps)
  {
    _fpsLabel->setText(_fps->fpsText());
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
            ILOG_ERROR( ILX_COMPOSITOR,
                "Error! GetDisplayLayer: %s", DirectFBErrorString(ret));
            return NULL;
          }

        IDirectFBWindow* window;
        ret = layer->GetWindow(layer, id, &window);
        if (ret)
          {
            layer->Release(layer);
            ILOG_ERROR( ILX_COMPOSITOR,
                "Error! GetWindow: %s", DirectFBErrorString(ret));
            return NULL;
          }
        layer->Release(layer);
        return window;
      }
    return NULL;
  }

  void
  Compositor::addWindow(AppInstance* instance, const SaWManWindowInfo* info)
  {
    ILOG_DEBUG( ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, info->win_id);

    CompositorEventData* data = new CompositorEventData;
    data->instance = instance;
    data->windowID = info->win_id;

    postUserEvent(CET_Add, data);
  }

  void
  Compositor::removeWindow(AppInstance* instance, const SaWManWindowInfo* info)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, info->win_id);

    CompositorEventData* data = new CompositorEventData;
    data->instance = instance;
    data->windowID = info->win_id;

    postUserEvent(CET_Remove, data);
  }

  void
  Compositor::configWindow(AppInstance* instance,
      SaWManWindowReconfig *reconfig, const SaWManWindowInfo* info)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %lu )\n", __FUNCTION__, info->win_id);

    CompositorEventData* data = new CompositorEventData;
    data->instance = instance;
    data->reconfig = new SaWManWindowReconfig;
    data->reconfig->caps = reconfig->caps;
    data->reconfig->current = reconfig->current;
    data->reconfig->flags = reconfig->flags;
    data->reconfig->handle = reconfig->handle;
    data->reconfig->request = reconfig->request;
    data->windowID = info->win_id;

    postUserEvent(CET_Config, data);
  }

  void
  Compositor::restackWindow(AppInstance* instance, const SaWManWindowInfo* info,
      int order, DFBWindowID other)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, info->win_id);

    CompositorEventData* data = new CompositorEventData;
    data->instance = instance;
    data->reconfig = new SaWManWindowReconfig;
    data->reconfig->caps = DWCAPS_NONE;
    data->reconfig->flags = SWMCF_STACKING;
    SaWManWindowConfig reconfig;
    reconfig.association = other;
    reconfig.opacity = order;
    data->reconfig->request = reconfig;
    data->windowID = info->win_id;

    postUserEvent(CET_Restack, data);
  }

  void
  Compositor::stateWindow(DFBWindowID id, const DFBWindowState* state)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, id);

//    CompositedAppRecord* record = lookUpAppRecordByWindowID(id);
//    if (record)
//      {
//        CompositorEventData* data = new CompositorEventData;
//        data->windowID = id;
//
//        postUserEvent(CET_State, data);
//      }
  }

  void
  Compositor::focusWindow(DFBWindowID id)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, id);

//    CompositedAppRecord* record = lookUpAppRecordByWindowID(id);
//    if (record)
//      {
//        CompositorEventData* data = new CompositorEventData;
//        data->windowID = id;
//
//        postUserEvent(CET_Focus, data);
//      }
  }

  void
  Compositor::processRemoved(AppInstance* instance)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( PID %u )\n", __FUNCTION__, instance->pid());

    CompositorEventData* data = new CompositorEventData;
    data->instance = instance;

    postUserEvent(CET_Quit, data);
  }

  void
  Compositor::updateCompositorGeometry()
  {
    _launcher->setGeometry(0, 0, width(), height());
    _switcher->setOptimalGeometry(width(), height());
    _homeButton->moveTo(0, 0);
    _switchButton->moveTo(0, height() - 80);
    _quitButton->moveTo(width() - 80, 0);
    if (_fpsLabel)
      _fpsLabel->setGeometry((width() - 100) / 2, 0, 100, height());
  }

  void
  Compositor::handleUserEvent(const DFBUserEvent& event)
  {
    if (event.clazz == DFEC_USER)
      {
        CompositorEventData* data = (CompositorEventData*) event.data;

        switch (event.type)
          {
        case CET_Add:
          {
            ILOG_DEBUG(ILX_COMPOSITOR, "CET_Add (%d)\n", data->windowID);
            IDirectFBWindow* dfbWindow = getWindow(data->windowID);
            if (!dfbWindow)
              break;
            AppInfo* appInfo = _appMan->infoByInstanceID(
                data->instance->instanceID());
            if (data->instance->view() == NULL)
              {
                data->instance->setView(
                    new AppView(this, data->instance, this));
                data->instance->view()->setGeometry(0, 0, width(), height());
                addWidget(data->instance->view());
                data->instance->view()->setNeighbour(Down, _switcher);
                data->instance->view()->setZ(-5);
                widgetToBack(data->instance->view());
              }

            if (data->instance->thumb() == NULL)
              {
                data->instance->setThumb(
                    new AppThumbnail(this, data->instance));
                _switcher->addThumb(data->instance->thumb());
              }

            data->instance->thumb()->addWindow(dfbWindow, false);
            data->instance->view()->addWindow(dfbWindow);
            _currentApp = data->instance;
            showLauncher(false);
            if (dfbWindow)
              dfbWindow->Release(dfbWindow);
          }
          break;

        case CET_Remove:
          {
            ILOG_DEBUG(ILX_COMPOSITOR, "CET_Remove (%d)\n", data->windowID);
            data->instance->thumb()->removeWindow(data->windowID);
            data->instance->view()->removeWindow(data->windowID);
          }
          break;

        case CET_Config:
          {
            ILOG_INFO(ILX_COMPOSITOR, "CET_CONFIG (%d)\n", data->windowID);

            data->instance->view()->onWindowConfig(data->windowID,
                data->reconfig);
            data->instance->thumb()->onWindowConfig(data->windowID,
                data->reconfig);

            delete data->reconfig;
          }
          break;

        case CET_Focus:
          break;

        case CET_Restack:
          {
            ILOG_INFO(ILX_COMPOSITOR, "CET_Restack (%d)\n", data->windowID);
            data->instance->view()->onWindowConfig(data->windowID,
                data->reconfig);
            data->instance->thumb()->onWindowConfig(data->windowID,
                data->reconfig);

            delete data->reconfig;
          }
          break;

        case CET_State:
          break;

        case CET_Quit:
          {
            _currentApp = NULL;
            if (data->instance->view())
              removeWidget(data->instance->view());
            if (data->instance->thumb())
              _switcher->removeThumb(data->instance->thumb());
            showLauncher(true);
          }
          break;

        default:
          break;
          }

        delete data;
      }
  }

  bool
  Compositor::windowPreEventFilter(const DFBWindowEvent& event)
  {
    switch (event.type)
      {
    case DWET_KEYDOWN:
      if (event.key_symbol == DIKS_HOME)
        {
          showLauncher(true); // show launcher
          return true;
        }
      else if (event.key_symbol == DIKS_TAB && event.modifiers == DIMM_ALT)
        {
          if (_appMan->instanceCount() < 2)
            return true;

          if (!_switcher->visible())
            showSwitcher(true);

          _switcher->scrollTo(_switcher->nextThumb());
          return true;
        }
      break;

    case DWET_KEYUP:
      {
        if (event.key_symbol == DIKS_ALT)
          {
            if (_appMan->instanceCount() < 2)
              return true;

            if (_switcher->visible())
              showSwitcher(false);
            return true;
          }
      }
      break;

    default:
      break;
      }
    return false;
  }

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
  try
    {
      ilixi::Compositor app(argc, argv);
      app.exec();
    }
  catch (const std::exception& e)
    {
      ILOG_FATAL(ilixi::ILX_COMPOSITOR, "Exception: %s\n", e.what());
    }
  return 0;
}
