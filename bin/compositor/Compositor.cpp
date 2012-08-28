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
          _previousApp(NULL),
          _switcher(NULL),
          _quitButton(NULL),
          _fpsLabel(NULL),
          _fps(NULL),
          _compComp(NULL),
          _soundComp(NULL),
          _oskComp(NULL),
          _home(NULL),
          _statusBar(NULL),
          _osk(NULL)
{
    setenv("WEBKIT_IGNORE_SSL_ERRORS", "1", 0);
    setenv("LITE_WINDOW_DOUBLEBUFFER", "1", 0);

    _appMan = new ApplicationManager(this);
    _soundComp = new SoundComponent();
    _compComp = new CompositorComponent(this);
    _oskComp = new OSKComponent(this);

    _showAnimProps = AppView::Opacity;
    _hideAnimProps = AppView::AnimatedProperty(
            AppView::Opacity | AppView::Zoom);

    setTitle("Compositor");
    setMargin(0);
    setPaletteFromFile(ILIXI_DATADIR"statusbar/def_palette.xml");

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "carousel") == 0)
            _switcher = new CarouselSwitcher();

        else if (strcmp(argv[i], "fps") == 0)
        {
            _fpsLabel = new Label("FPS: 0");
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
    delete _oskComp;
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
    if (show)
    {
        ILOG_TRACE_W(ILX_COMPOSITOR);
        showCurrentApp(false);
        _backgroundFlags = BGFAll;
        _home->view()->show(_showAnimProps);
        _compComp->signalHome(true);
        showSwitcher(false);
    } else if (_currentApp)
    {
        _home->view()->hide(_hideAnimProps);
        _compComp->signalHome(false);
        showCurrentApp(true);
        showSwitcher(false);
    }
}

void
Compositor::showSwitcher(bool show)
{
    if (_switcher->itemCount() == 0)
        return;

    ILOG_TRACE_W(ILX_COMPOSITOR);
    if (show)
    {
        _switcher->show();
        _compComp->signalSwitcher(true);
        eventManager()->setGrabbedWidget(NULL);
        hideOSK();
    } else
    {
        _switcher->hide();
        _compComp->signalSwitcher(false);
    }
}

void
Compositor::handleViewRequest(AppInstance* instance)
{
    _previousApp = _currentApp;
    if (_previousApp)
        _previousApp->view()->hide(_hideAnimProps);
    _currentApp = instance;
    showLauncher(false);
}

void
Compositor::handleSwitchRequest()
{
    if (_currentApp)
    {
        _currentApp->view()->hide(_hideAnimProps);
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
    if (_home->view()->visible())
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
Compositor::showCurrentApp(bool show)
{
    if (_currentApp)
    {
        if (show)
        {
            _switcher->setNeighbour(Up, _currentApp->view());
            _currentApp->view()->clearAnimatedProperty(AppView::Position);
            _currentApp->view()->show(_showAnimProps);
            _compComp->notifyVisible(_currentApp->pid());

            AppInfo* info = _currentApp->appInfo();
            if (info->appFlags() & APP_NEEDS_CLEAR)
                _backgroundFlags = BGFAll;
            else
                _backgroundFlags = BGFNone;
        } else
        {
            if (_osk && _osk->view()->visible())
            {
                _osk->view()->hide(AppView::Position, 0, height());
                _currentApp->view()->hide(
                        AppView::AnimatedProperty(
                                _hideAnimProps | AppView::Position),
                        0, 0);
            } else
                _currentApp->view()->hide(_hideAnimProps);
            _compComp->notifyHidden(_currentApp->pid());
        }
    }
}

void
Compositor::showOSK(DFBRectangle rect)
{
    if (!_osk)
        _appMan->startApp("OnScreenKeyboard");
    else if (!_osk->view()->visible())
        _osk->view()->show(AppView::Position, 0, height() - 450);
    else
        update();

    if (rect.y > height() - 450)
    {
        rect.y = rect.y + rect.h - (height() - 450);
        _currentApp->view()->slideTo(0, -rect.y);
    }
}

void
Compositor::hideOSK()
{
    if (_osk)
    {
        _currentApp->view()->slideTo(0, 0);
        _osk->view()->hide(AppView::Position, 0, height());
    }
}

void
Compositor::sendOSKInput(uint32_t key)
{
    _currentApp->view()->consumeKeyEvent(
            KeyEvent(KeyDownEvent, (DFBInputDeviceKeySymbol) key));
}

void
Compositor::showSound(bool show)
{
    if (show)
    {
        _appMan->startApplication("SoundMixer");
        _compComp->signalSound(true);
        _compComp->signalDash(false);
    } else
    {
        if (_previousApp && _previousApp != _currentApp)
        {
            _currentApp->view()->hide(_hideAnimProps);
            _currentApp = _previousApp;
            _currentApp->view()->show(_showAnimProps);
        } else
            showLauncher(true);
        _compComp->signalSound(false);
    }
}

void
Compositor::showDash(bool show)
{
    if (show)
    {
        _appMan->startApplication("Dashboard");
        _compComp->signalDash(true);
        _compComp->signalSound(false);
    } else
    {
        if (_previousApp && _previousApp != _currentApp)
        {
            _currentApp->view()->hide(_hideAnimProps);
            _currentApp = _previousApp;
            _currentApp->view()->show(_showAnimProps);
        } else
            showLauncher(true);
        _compComp->signalDash(false);
    }
}

void
Compositor::compose(const PaintEvent& event)
{
    Painter painter(this);
    painter.begin(event);
    painter.stretchImage(background(), 0, 0, width(), height() - 50);
    painter.end();
}

void
Compositor::onVisible()
{
    _quitButton->show();

    if (_fps)
    {
        _fps->start();
        _fpsLabel->bringToFront();
    }

    _appMan->initStartup();
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
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, info->win_id);

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
                    _statusBar = data->instance;
                    _statusBar->setView(
                            new AppView(this, data->instance, this));
                    _statusBar->view()->setGeometry(0, height() - 55, width(),
                                                    55);
                    addWidget(_statusBar->view());
                    _statusBar->view()->setZ(0);
                    _statusBar->view()->bringToFront();
                    _statusBar->view()->clearAnimatedProperty(AppView::Opacity);
                    _statusBar->view()->clearAnimatedProperty(AppView::Zoom);
                    _statusBar->view()->addWindow(dfbWindow);
                    _statusBar->view()->show();
                } else if (appInfo->appFlags() & APP_OSK)
                {
                    _osk = data->instance;
                    _osk->setView(new AppView(this, data->instance, this));
                    _osk->view()->setAnimatedProperty(AppView::Position);
                    _osk->view()->clearAnimatedProperty(AppView::Opacity);
                    _osk->view()->clearAnimatedProperty(AppView::Zoom);
                    _osk->view()->setGeometry(0, height() - 450, width(), 400);
                    addWidget(_osk->view());
                    _statusBar->view()->bringToFront();
                    _osk->view()->setZ(0);
                    _osk->view()->addWindow(dfbWindow);
                    _osk->view()->show(AppView::Position, 0, height() - 450);
                } else if (appInfo->appFlags() & APP_HOME)
                {
                    _home = data->instance;
                    _home->setView(new AppView(this, _home, this));
                    _home->view()->setGeometry(0, 0, width(), height() - 50);
                    addWidget(_home->view());
                    _home->view()->setZ(0);
                    _home->view()->sendToBack();
                    _home->view()->addWindow(dfbWindow);
                } else if (appInfo->appFlags() & APP_SYSTEM)
                {
                    data->instance->setView(
                            new AppView(this, data->instance, this));
                    data->instance->view()->setGeometry(0, 0, width(),
                                                        height() - 50);
                    addWidget(data->instance->view());
                    data->instance->view()->setZ(0);
                    data->instance->view()->sendToBack();
                    data->instance->view()->addWindow(dfbWindow);
                    data->instance->view()->show(_showAnimProps);
                    _previousApp = _currentApp;
                    if (_previousApp)
                        _previousApp->view()->hide(_hideAnimProps);
                    _currentApp = data->instance;

                    if (_currentApp->windowCount() < 2)
                        showLauncher(false);
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
                        data->instance->view()->sendToBack();
                    }

                    if (data->instance->thumb() == NULL)
                    {
                        data->instance->setThumb(
                                new AppThumbnail(this, data->instance));
                        _switcher->addThumb(data->instance->thumb());
                    }

                    AppInfo* info = data->instance->appInfo();
                    data->instance->thumb()->addWindow(dfbWindow, false);
                    data->instance->view()->addWindow(
                            dfbWindow, true,
                            !(info->appFlags() & APP_SURFACE_DONTBLOCK));

                    if (!(info->appFlags() & APP_AUTO_START))
                    {
                        _previousApp = _currentApp;
                        _currentApp = data->instance;

                        if (_currentApp->windowCount() < 2)
                            showLauncher(false);
                    }
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
        } else if (event.key_symbol == DIKS_F11)
        {
            Notify notify("Hey!", "Want parmak?");
            notify.setIcon(ILIXI_DATADIR"images/default.png");
            notify.show();
            return true;
        } else if (event.key_symbol == DIKS_ESCAPE)
            return true;
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
