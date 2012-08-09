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

#include "Compositor.h"
#include "switcher/HorizontalSwitcher.h"
#include "switcher/CarouselSwitcher.h"
#include "component/Notification.h"
#include <graphics/Painter.h>
#include <core/Logger.h>
#include <sigc++/bind.h>
#include "OSKView.h"

using namespace std;

namespace ilixi
{
D_DEBUG_DOMAIN( ILX_COMPOSITOR, "ilixi/compositor", "Compositor");

Compositor::Compositor(int argc, char* argv[])
        : Application(&argc, &argv, (AppOptions) (OptExclusive | OptDale)),
          _appMan(NULL),
          _currentApp(NULL),
          _switcher(NULL),
          _launcher(NULL),
          _quitButton(NULL),
          _fpsLabel(NULL),
          _fps(NULL),
          _compComp(NULL),
          _soundComp(NULL),
          _oskComp(NULL)
{
    _appMan = new ApplicationManager(this);
    _soundComp = new SoundComponent();
    _compComp = new CompositorComponent(this);
//    _oskComp = new OSKComponent(this);

    setTitle("Compositor");
    setBackgroundImage(ILIXI_DATADIR"compositor/bg.png");
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
        }

        else if (strcmp(argv[i], "fsu") == 0)
            setAppOption(OptFullScreenUpdate);
    }

    if (_switcher == NULL)
        _switcher = new HorizontalSwitcher();

    _switcher->sigSwitchRequest.connect(
            sigc::mem_fun(this, &Compositor::handleSwitchRequest));
    addWidget(_switcher);

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
    delete _compComp;
    delete _soundComp;
//    delete _oskComp;
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
        {
            _currentApp->view()->hide();
            _compComp->notifyHidden(_currentApp->pid());
        }
        _backgroundFlags = BGFAll;
        _launcher->setVisible(true);
        _compComp->signalHomeShowing();
        showSwitcher(false);
    } else
    {
        if (_currentApp)
        {
            _switcher->setNeighbour(Up, _currentApp->view());
            _currentApp->view()->show();
            _compComp->notifyVisible(_currentApp->pid());
            AppInfo* info = _appMan->infoByAppID(_currentApp->appID());
            if (info->appFlags() & APP_NEEDS_CLEAR)
                _backgroundFlags = BGFAll;
            else
                _backgroundFlags = BGFNone;
        } else
            _backgroundFlags = BGFAll;

        _launcher->setVisible(false);
        _compComp->signalHomeHidden();
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
        _switcher->show();
        _compComp->signalSwitcherShowing();
        eventManager()->setGrabbedWidget(NULL);
    } else
    {
        _switcher->hide();
        _compComp->signalSwitcherHidden();
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
    {
        _currentApp->view()->hide();
        _compComp->notifyHidden(_currentApp->pid());
    }

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
Compositor::addOverlay(DFBSurfaceID id)
{
}

void
Compositor::addDialog(DFBSurfaceID id)
{
}

void
Compositor::showOSK(DFBRectangle rect)
{
}

void
Compositor::hideOSK()
{
}

void
Compositor::compose(const PaintEvent& event)
{
    Painter painter(this);
    painter.begin(event);
    painter.drawImage(background(), 0, 0, width(), height() - 50);
    painter.end();
}

void
Compositor::onVisible()
{
    _quitButton->show();

    if (_fps)
        _fps->start();

    _appMan->startApp("StatusBar");
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
Compositor::configWindow(AppInstance* instance, SaWManWindowReconfig *reconfig,
                         const SaWManWindowInfo* info)
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
    _switcher->setOptimalGeometry(width(), height() - 50);
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
        AppInfo* appInfo = _appMan->infoByInstanceID(
                data->instance->instanceID());

        switch (event.type)
        {
        case CET_Add:
            {
                ILOG_DEBUG(ILX_COMPOSITOR, "CET_Add (%d)\n", data->windowID);
                IDirectFBWindow* dfbWindow = getWindow(data->windowID);
                if (!dfbWindow)
                    break;

                if (appInfo->appFlags() & APP_STATUSBAR)
                {
                    data->instance->setView(
                            new AppView(this, data->instance, this));
                    data->instance->view()->setGeometry(0, height() - 54,
                                                        width(), 54);
                    addWidget(data->instance->view());
                    data->instance->view()->setZ(0);
                    widgetToFront(data->instance->view());

                    data->instance->view()->addWindow(dfbWindow);
//                    _appMan->startApp("OnScreenKeyboard");
                } else if (appInfo->appFlags() & APP_OSK)
                {
                    _osk = data->instance;
//                    data->instance->setView(
//                            new AppView(this, data->instance, this));
//                    data->instance->view()->setGeometry(0, height() - 400,
//                                                        width(), 400);
//                    addWidget(data->instance->view());
//                    data->instance->view()->setZ(0);
//                    widgetToFront(data->instance->view());
//
//                    data->instance->view()->addWindow(dfbWindow);
//                    data->instance->view()->show();

                    OSKView* oskView = new OSKView(this, data->instance, this);
                    oskView->setGeometry(0, 0, width(), height());
                    addWidget(oskView);
                    widgetToFront(oskView);
                    oskView->addWindow(dfbWindow);
                    oskView->show();

//                    SurfaceView* osk = new SurfaceView();
//                    osk->setSourceFromWindow(dfbWindow);
//                    osk->setGeometry(0, height() - 400, width(), 400);
//                    addWidget(osk);
//                    widgetToFront(osk);
                } else
                {
                    if (data->instance->view() == NULL)
                    {
                        data->instance->setView(
                                new AppView(this, data->instance, this));
                        data->instance->view()->setGeometry(0, 0, width(),
                                                            height() - 50);
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

                    AppInfo* info = _appMan->infoByAppID(
                            data->instance->appID());
                    data->instance->thumb()->addWindow(dfbWindow, false);
                    data->instance->view()->addWindow(
                            dfbWindow, true,
                            !(info->appFlags() & APP_SURFACE_DONTBLOCK));
                    _currentApp = data->instance;
                    showLauncher(false);
                }
                if (dfbWindow)
                    dfbWindow->Release(dfbWindow);
            }
            break;

        case CET_Remove:
            {
                ILOG_DEBUG(ILX_COMPOSITOR, "CET_Remove (%d)\n", data->windowID);

                if (data->instance->view())
                    data->instance->view()->removeWindow(data->windowID);

                if (data->instance->thumb())
                    data->instance->thumb()->removeWindow(data->windowID);
            }
            break;

        case CET_Config:
            {
                ILOG_DEBUG(ILX_COMPOSITOR, "CET_CONFIG (%d)\n", data->windowID);

                if (data->instance->view())
                    data->instance->view()->onWindowConfig(data->windowID,
                                                           data->reconfig);
                if (data->instance->thumb())
                    data->instance->thumb()->onWindowConfig(data->windowID,
                                                            data->reconfig);

                delete data->reconfig;
            }
            break;

        case CET_Focus:
            break;

        case CET_Restack:
            {
                ILOG_DEBUG(ILX_COMPOSITOR,
                           "CET_Restack (%d)\n", data->windowID);

                if (data->instance->view())
                    data->instance->view()->onWindowConfig(data->windowID,
                                                           data->reconfig);
                if (data->instance->thumb())
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
        } else if (event.key_symbol == DIKS_TAB && event.modifiers == DIMM_ALT)
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
    } catch (const std::exception& e)
    {
        ILOG_FATAL(ilixi::ILX_COMPOSITOR, "Exception: %s\n", e.what());
    }
    return 0;
}
