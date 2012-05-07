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
#include "graphics/Painter.h"
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
      Application(argc, argv, OptExclusive), _currentApp(NULL)
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

    _launcher = new Launcher(this);
    addWidget(_launcher);

    _switcher = new Switcher();
    _switcher->sigSwitchRequest.connect(
        sigc::mem_fun(this, &Compositor::handleSwitchRequest));
    addWidget(_switcher);

    _homeButton = new HomeButton();
    _homeButton->sigPressed.connect(
        sigc::bind<bool>(sigc::mem_fun(this, &Compositor::showLauncher), true));
    addWidget(_homeButton);

    _switchButton = new SwitchButton();
    _switchButton->setVisible(false);
    _switchButton->sigPressed.connect(
        sigc::bind<bool>(sigc::mem_fun(this, &Compositor::showSwitcher), true));
    addWidget(_switchButton);

    _quitButton = new QuitButton();
    _quitButton->sigPressed.connect(
        sigc::mem_fun(this, &Compositor::handleQuit));
    addWidget(_quitButton);

    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &Compositor::updateCompositorGeometry));
    sigVisible.connect(sigc::mem_fun(this, &Compositor::onVisible));

    ApplicationManager::instance()->sigViewRequest.connect(
        sigc::mem_fun(this, &Compositor::handleViewRequest));
  }

  Compositor::~Compositor()
  {
    ApplicationManager::instance()->stopAll();
    _iWindows->Release(_iWindows);
    _iWindows = NULL;
    ILOG_INFO(ILX_COMPOSITOR, "~Compositor\n");
  }

  CompositedAppRecord*
  Compositor::lookUpAppRecord(unsigned long appID)
  {
    for (AppVector::iterator it = _appVector.begin(); it != _appVector.end();
        ++it)
      if (((CompositedAppRecord*) *it)->appID == appID)
        return *it;
    return NULL;
  }

  CompositedAppRecord*
  Compositor::lookUpAppRecord(unsigned int instanceID)
  {
    for (AppVector::iterator it = _appVector.begin(); it != _appVector.end();
        ++it)
      if (((CompositedAppRecord*) *it)->instanceID == instanceID)
        return *it;
    return NULL;
  }

  CompositedAppRecord*
  Compositor::lookUpAppRecordUsingWindowID(DFBWindowID windowID)
  {
    for (AppVector::iterator it = _appVector.begin(); it != _appVector.end();
        ++it)
      {
        for (std::vector<DFBWindowID>::iterator wit =
            ((CompositedAppRecord*) *it)->_windows.begin();
            wit != ((CompositedAppRecord*) *it)->_windows.end(); ++wit)
          {
            if (*wit == windowID)
              return *it;
          }
      }
    return NULL;
  }

  void
  Compositor::addAppRecord(CompositedAppRecord* appRecord)
  {
    _appVector.push_back(appRecord);
  }

  void
  Compositor::removeAppRecord(unsigned long appID)
  {
    for (AppVector::iterator it = _appVector.begin(); it != _appVector.end();
        ++it)
      if (((CompositedAppRecord*) *it)->appID == appID)
        {
          _appVector.erase(it);
          pthread_mutex_destroy(&((CompositedAppRecord*) *it)->mutex);
          delete *it;
          break;
        }
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
  Compositor::showLauncher(bool show)
  {
    if (show && !_launcher->visible())
      {
        if (_currentApp)
          _currentApp->view->hide();
        _backgroundFlags = BGFAll;
        _launcher->setVisible(true);
        _homeButton->hide();
        showSwitcher(false);
      }
    else
      {
        if (_currentApp)
          _currentApp->view->show();
        _backgroundFlags = BGFNone;
        _launcher->setVisible(false);
        _homeButton->show();
        showSwitcher(false);
      }
  }

  void
  Compositor::showSwitcher(bool show)
  {
    if (_appVector.size() < 2)
      return;

    if (show)
      {
        _switcher->show();
        _switchButton->hide();
      }
    else
      {
        _switcher->hide();
        _switchButton->show();
      }
  }

  void
  Compositor::handleViewRequest(unsigned long appID)
  {
    CompositedAppRecord* appRecord = lookUpAppRecord(appID);
    _currentApp = appRecord;
    showLauncher(false);
  }

  void
  Compositor::handleSwitchRequest()
  {
    CompositedAppRecord* record;
    for (AppVector::iterator it = _appVector.begin(); it != _appVector.end();
        ++it)
      if (((CompositedAppRecord*) *it)->thumb == _switcher->currentThumb())
        record = ((CompositedAppRecord*) *it);

    if (record)
      _currentApp = record;

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
        removeWidget(_currentApp->view);
        _switcher->removeThumb(_currentApp->thumb);
        removeAppRecord(_currentApp->appID);
        ApplicationManager::instance()->stopApplication(_currentApp->appID);
        _currentApp = NULL;
        showLauncher(true);
      }
  }

  void
  Compositor::onVisible()
  {
    _quitButton->show();
  }

  void
  Compositor::addWindow(const DFBWindowInfo* info)
  {
    ILOG_DEBUG( ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, info->window_id);

    ILOG_DEBUG(ILX_COMPOSITOR, "  -> caps         0x%08x\n", info->caps);
    ILOG_DEBUG( ILX_COMPOSITOR,
        "  -> res. id  0x%016llx\n", (unsigned long long) info->resource_id);
    ILOG_DEBUG( ILX_COMPOSITOR,
        "  -> Instance: %u PID: %d\n", info->instance_id, info->process_id);

    CompositedAppRecord* appRecord = lookUpAppRecord(info->instance_id);

    if (!appRecord)
      {
        appRecord = new CompositedAppRecord;
        appRecord->instanceID = info->instance_id;
        appRecord->processID = info->process_id;
        pthread_mutex_init(&appRecord->mutex, NULL);
        _appVector.push_back(appRecord);

        AppInstanceRecord* appInstanceRecord =
            ApplicationManager::instance()->lookUpAppInstanceRecord(
                (int) info->process_id);

        if (appInstanceRecord)
          appRecord->appID = appInstanceRecord->appID;

        AppInfo* info = ApplicationManager::instance()->lookUpAppRecord(
            appRecord->appID);

        if (info)
          {
            appRecord->thumb = new AppThumbnail(info->name, info->appFlags);
            appRecord->view = new AppView(this, info->appFlags);
          }
        else
          {
            appRecord->thumb = new AppThumbnail("N/A");
            appRecord->view = new AppView(this);
          }

        _switcher->addThumb(appRecord->thumb);
        appRecord->view->setGeometry(0, 0, width(), height());
        addWidget(appRecord->view);
        widgetToBack(appRecord->view);
        ILOG_DEBUG( ILX_COMPOSITOR, "Created new AppRecord\n");
      }

    CompositorEventData* data = new CompositorEventData;
    data->appRecord = appRecord;
    data->windowID = info->window_id;
    appRecord->_windows.push_back(info->window_id);

    postUserEvent(CET_Add, data);
  }

  void
  Compositor::removeWindow(DFBWindowID id)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, id);

    CompositedAppRecord* record = lookUpAppRecordUsingWindowID(id);
    if (record)
      {
        CompositorEventData* data = new CompositorEventData;
        data->appRecord = record;
        data->windowID = id;

        postUserEvent(CET_Remove, data);
      }
  }

  void
  Compositor::configWindow(DFBWindowID id, const DFBWindowConfig *config,
      DFBWindowConfigFlags flags)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, id);
    ILOG_DEBUG(ILX_COMPOSITOR, "  -> flags        0x%08x\n", flags);

    CompositedAppRecord* record = lookUpAppRecordUsingWindowID(id);
    if (record)
      {
        CompositorEventData* data = new CompositorEventData;
        data->appRecord = record;
        data->windowID = id;
        data->config = config;
        data->flags = flags;

        postUserEvent(CET_Config, data);
      }
  }

  void
  Compositor::stateWindow(DFBWindowID id, const DFBWindowState* state)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, id);
    ILOG_DEBUG(ILX_COMPOSITOR, "  -> flags        0x%08x\n", state->flags);

    CompositedAppRecord* record = lookUpAppRecordUsingWindowID(id);
    if (record)
      {
        CompositorEventData* data = new CompositorEventData;
        data->appRecord = record;
        data->windowID = id;

        postUserEvent(CET_State, data);
      }
  }

  void
  Compositor::restackWindow(DFBWindowID id, unsigned int index)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, id);
    ILOG_DEBUG(ILX_COMPOSITOR, "  -> index        %u\n", index);

    CompositedAppRecord* record = lookUpAppRecordUsingWindowID(id);
    if (record)
      {
        CompositorEventData* data = new CompositorEventData;
        data->appRecord = record;
        data->windowID = id;

        postUserEvent(CET_Restack, data);
      }
  }

  void
  Compositor::focusWindow(DFBWindowID id)
  {
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, id);

    CompositedAppRecord* record = lookUpAppRecordUsingWindowID(id);
    if (record)
      {
        CompositorEventData* data = new CompositorEventData;
        data->appRecord = record;
        data->windowID = id;

        postUserEvent(CET_Focus, data);
      }
  }

  void
  Compositor::updateCompositorGeometry()
  {
    _launcher->setGeometry(0, 0, width(), height());
    _switcher->setGeometry(0, height() - HS_H, width(), HS_H);
    _homeButton->moveTo(0, 0);
    _switchButton->moveTo(0, height() - 80);
    _quitButton->moveTo(width() - 80, 0);
  }

  void
  Compositor::handleUserEvent(const DFBUserEvent& event)
  {
    if (_iWindows && event.clazz == DFEC_USER)
      {
        CompositorEventData* data = (CompositorEventData*) event.data;
        IDirectFBWindow* dfbWindow = getWindow(data->windowID);

        switch (event.type)
          {
        case CET_Add:
          {
            _currentApp = data->appRecord;

            ILOG_DEBUG(ILX_COMPOSITOR, "CET_Add (%d)\n", data->windowID);
            dfbWindow->Move(dfbWindow, 10000, 0);
            dfbWindow->SetOpacity(dfbWindow, 0);
            data->appRecord->thumb->addWindow(dfbWindow);
            data->appRecord->view->addWindow(dfbWindow);

            showLauncher(false);
          }
          break;

        case CET_Remove:
          ILOG_DEBUG(ILX_COMPOSITOR, "CET_Remove (%d)\n", data->windowID);
          data->appRecord->thumb->removeWindow(dfbWindow);
          data->appRecord->view->removeWindow(dfbWindow);
          break;

        case CET_Config:
          {
            ILOG_INFO(ILX_COMPOSITOR, "CET_CONFIG (%d)\n", data->windowID);

            if (data->appRecord && data->appRecord->view)
              {
                data->appRecord->view->onWindowConfig(data->windowID,
                    data->config, data->flags);
                data->appRecord->thumb->onWindowConfig(data->windowID,
                    data->config, data->flags);
              }
          }
          break;

        case CET_Focus:
          break;

        case CET_Restack:
          break;

        case CET_State:
          break;

        default:
          break;
          }

        if (dfbWindow)
          dfbWindow->Release(dfbWindow);
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
          if (_appVector.size() < 2)
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
            if (_appVector.size() < 2)
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

  void
  Compositor::compose(const Rectangle& rect)
  {
    if (_launcher->visible())
      {
        Painter painter(this);
        painter.begin(rect);
        stylist()->drawAppFrame(&painter, this);
        painter.end();
      }
  }

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
  ilixi::Compositor app(argc, argv);
  app.exec();
  return 0;
}
