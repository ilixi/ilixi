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
#include <lib/Notify.h>
#include <sigc++/bind.h>

using namespace std;

namespace ilixi
{
D_DEBUG_DOMAIN( ILX_COMPOSITOR, "ilixi/comp/Compositor", "Compositor");

ILXCompositor::CompositorSettings ILXCompositor::settings;

ILXCompositor::ILXCompositor(int argc, char* argv[])
        : Application(&argc, &argv,
                      (AppOptions) (OptExclusive | OptDale | OptSound)),
          _appMan(NULL),
          _currentApp(NULL),
          _previousApp(NULL),
          _switcher(NULL),
          _fpsLabel(NULL),
          _fps(NULL),
          _compComp(NULL),
          _soundComp(NULL),
          _oskComp(NULL),
          _home(NULL),
          _statusBar(NULL),
          _osk(NULL),
          _dash(NULL),
          _mixer(NULL),
          _oskTargetPID(0)
{
    setenv("WEBKIT_IGNORE_SSL_ERRORS", "1", 0);
    setenv("LITE_WINDOW_DOUBLEBUFFER", "1", 0);

    _soundComp = new SoundComponent();
    _compComp = new CompositorComponent(this);
    _oskComp = new OSKComponent(this);

    _appMan = new ApplicationManager(this);

    settings.clickFocus = true;
    settings.animations = true;
    settings.showAnimProps = AppView::Opacity;
    settings.hideAnimProps = AppView::AnimatedProperty(
            AppView::Opacity | AppView::Zoom);
    settings.durationShow = 300;
    settings.durationHide = 500;
    settings.notificationTimeout = 5000;

    setTitle("Compositor");
    setMargin(0);
    setPaletteFromFile(ILIXI_DATADIR"statusbar/def_palette.xml");
    _backgroundFlags = BGFNone;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "carousel") == 0)
            _switcher = new CarouselSwitcher();

        else if (strcmp(argv[i], "fps") == 0)
        {
            _backgroundFlags = BGFFill;
            _fpsLabel = new Label("FPS: 0");
            _fpsLabel->layout().setAlignment(TextLayout::Center);
            addWidget(_fpsLabel);

            _fps = new FPSCalculator();
            _fps->sigUpdated.connect(
                    sigc::mem_fun(this, &ILXCompositor::onFPSUpdate));
        }

        else if (strcmp(argv[i], "fsu") == 0)
            setAppOption(OptFullScreenUpdate);
    }

    if (_switcher == NULL)
        _switcher = new HorizontalSwitcher();

    _switcher->sigSwitchRequest.connect(
            sigc::mem_fun1(this, &ILXCompositor::showInstance));
    _switcher->sigFeedbackRequest.connect(
            sigc::mem_fun(_compComp, &CompositorComponent::notifyVisibility));
    addWidget(_switcher);

    sigGeometryUpdated.connect(
            sigc::mem_fun(this, &ILXCompositor::updateCompositorGeometry));
    sigVisible.connect(sigc::mem_fun(this, &ILXCompositor::onVisible));
}

ILXCompositor::~ILXCompositor()
{
    ILOG_TRACE_W(ILX_COMPOSITOR);
    delete _appMan;
    delete _fps;
    delete _compComp;
    delete _soundComp;
    delete _oskComp;
}

ApplicationManager*
ILXCompositor::appMan() const
{
    return _appMan;
}

void
ILXCompositor::showInstance(AppInstance* instance)
{
    ILOG_TRACE_W(ILX_COMPOSITOR);
    toggleSwitcher(false);
    if (!instance || !instance->view())
    {
        toggleLauncher(true);
        return;
    } else if (instance == _currentApp)
        return;

    _previousApp = _currentApp;
    _currentApp = instance;

    ILOG_DEBUG(ILX_COMPOSITOR,
               " -> instance: %s\n", instance->appInfo()->name().c_str());

    // osk & _previousApp
    if (_osk && _osk->view()->visible())
    {
        _osk->view()->hide(AppView::Position, 0, height());
        if (_previousApp)
            _previousApp->view()->hide(
                    AppView::AnimatedProperty(
                            settings.hideAnimProps | AppView::Position),
                    0, 0);
    } else if (_previousApp)
        _previousApp->view()->hide(settings.hideAnimProps);

    // background
    AppInfo* info = _currentApp->appInfo();
    if (info->appFlags() & APP_NEEDS_CLEAR)
        _backgroundFlags |= BGFClear;
    else
        _backgroundFlags &= ~BGFClear;

    ILOG_INFO(ILX_COMPOSITOR, "NOW SHOWING: %s\n", info->name().c_str());
    _currentApp->view()->show(settings.showAnimProps);
    _compComp->signalInstanceChanged(_currentApp, _previousApp);

    if (info->appFlags() & APP_USE_BACK)
        _compComp->signalBack(true);
    else
        _compComp->signalBack(false);
}

void
ILXCompositor::toggleLauncher(bool show)
{
    ILOG_TRACE_W(ILX_COMPOSITOR);
    if (show)
        showInstance(_home);
    else if (_previousApp)
        showInstance(_previousApp);
}

void
ILXCompositor::toggleSwitcher(bool show)
{
    ILOG_TRACE_W(ILX_COMPOSITOR);
    if (show && _switcher->itemCount() > 0)
    {
        _switcher->show();
        _compComp->signalSwitcher(true);
        eventManager()->setGrabbedWidget(NULL);
        toggleOSK(false);
    } else
    {
        _switcher->hide();
        _compComp->signalSwitcher(false);
    }
}

void
ILXCompositor::killApp(AppInstance* instance)
{
    if (instance == _currentApp)
    {
        _currentApp = NULL;
        toggleLauncher(true);
    }

    if (instance == _previousApp)
        _previousApp = NULL;

    CompositorEventData* data = new CompositorEventData;
    data->instance = instance;

    postUserEvent(CET_Term, data);
}

void
ILXCompositor::showOSK(DFBRectangle rect, pid_t process)
{
    ILOG_TRACE_W(ILX_COMPOSITOR);
    ILOG_DEBUG(ILX_COMPOSITOR, " -> process: %d\n", process);
    if (rect.y > height() - 450)
        _oskTargetRect.setRectangle(rect.x, rect.y + rect.h - (height() - 450),
                                    rect.w, rect.h);
    else
        _oskTargetRect.setRectangle(rect.x, 0, rect.w, rect.h);

    _oskTargetPID = process;
    if (!_osk)
        _appMan->startApp("OnScreenKeyboard");
    else
        toggleOSK(true);
}

void
ILXCompositor::toggleOSK(bool show)
{
    if (_osk && _currentApp->pid() == _oskTargetPID)
    {
        ILOG_TRACE_W(ILX_COMPOSITOR);
        if (show)
        {
            _currentApp->view()->slideTo(0, -_oskTargetRect.y());
            _osk->view()->show(AppView::Position, 0, height() - 450);
            toggleSwitcher(false);
        } else
        {
            _currentApp->view()->slideTo(0, 0);
            _osk->view()->hide(AppView::Position, 0, height());
            _oskTargetPID = 0;
        }
    }
}

void
ILXCompositor::sendOSKInput(uint32_t key)
{
    _currentApp->view()->consumeKeyEvent(
            KeyEvent(KeyDownEvent, (DFBInputDeviceKeySymbol) key));
    if (key == DIKS_ENTER)
        toggleOSK(false);
}

void
ILXCompositor::showSound(bool show)
{
    if (show)
    {
        if (!_mixer)
            _appMan->startApplication("SoundMixer");
        else
            showInstance(_mixer);
    } else
        showInstance(_previousApp);
}

void
ILXCompositor::showDash(bool show)
{
    if (show)
    {
        if (!_dash)
            _appMan->startApplication("Dashboard");
        else
            showInstance(_dash);
    } else
        showInstance(_previousApp);
}

void
ILXCompositor::setLayerOpacity(u8 opacity)
{
    ILOG_TRACE_W(ILX_COMPOSITOR);
    ILOG_DEBUG(ILX_COMPOSITOR, " -> %d\n", opacity);
    IDirectFBDisplayLayer* layer;
    DFBResult ret = getDFB()->GetDisplayLayer(getDFB(), DLID_PRIMARY, &layer);
    layer->SetOpacity(layer, opacity);
    layer->Release(layer);
}

void
ILXCompositor::compose(const PaintEvent& event)
{
    if (_fps)
        _fps->funck();
}

void
ILXCompositor::onVisible()
{
    if (_fps)
        _fpsLabel->bringToFront();

    _appMan->initStartup();
}

void
ILXCompositor::onFPSUpdate(float fps)
{
    _fpsLabel->setText(_fps->fpsText());
}

IDirectFBWindow*
ILXCompositor::getWindow(DFBWindowID id)
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
ILXCompositor::addWindow(AppInstance* instance, const SaWManWindowInfo* info)
{
    ILOG_DEBUG( ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, info->win_id);

    CompositorEventData* data = new CompositorEventData;
    data->instance = instance;
    data->windowID = info->win_id;

    postUserEvent(CET_Add, data);
}

void
ILXCompositor::removeWindow(AppInstance* instance, const SaWManWindowInfo* info)
{
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( ID %u )\n", __FUNCTION__, info->win_id);

    CompositorEventData* data = new CompositorEventData;
    data->instance = instance;
    data->windowID = info->win_id;

    postUserEvent(CET_Remove, data);
}

void
ILXCompositor::configWindow(AppInstance* instance, SaWManWindowReconfig *reconfig, const SaWManWindowInfo* info)
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
ILXCompositor::restackWindow(AppInstance* instance, const SaWManWindowInfo* info, int order, DFBWindowID other)
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
ILXCompositor::stateWindow(DFBWindowID id, const DFBWindowState* state)
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
ILXCompositor::focusWindow(DFBWindowID id)
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
ILXCompositor::processRemoved(AppInstance* instance)
{
    ILOG_DEBUG(ILX_COMPOSITOR, "%s( PID %u )\n", __FUNCTION__, instance->pid());

    CompositorEventData* data = new CompositorEventData;
    data->instance = instance;

    postUserEvent(CET_Quit, data);
}

void
ILXCompositor::updateCompositorGeometry()
{
    _switcher->setOptimalGeometry(width(), height() - 50);

    if (_fpsLabel)
        _fpsLabel->setGeometry((width() - 100) / 2, 0, 100, height());
}

void
ILXCompositor::handleUserEvent(const DFBUserEvent& event)
{
    if (event.clazz == DFEC_USER)
    {
        CompositorEventData* data = (CompositorEventData*) event.data;
        AppInfo* appInfo = data->instance->appInfo();

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
                    ILOG_DEBUG(ILX_COMPOSITOR, " -> APP_STATUSBAR\n");
                    _statusBar = data->instance;
                    _statusBar->setView(
                            new AppView(this, data->instance, this));
                    _statusBar->view()->setGeometry(0, height() - 55, width(),
                                                    55);
                    addWidget(_statusBar->view());
                    _statusBar->view()->setZ(0);
                    _statusBar->view()->bringToFront();
                    _statusBar->view()->addWindow(dfbWindow);
                } else if (appInfo->appFlags() & APP_OSK)
                {
                    ILOG_DEBUG(ILX_COMPOSITOR, " -> APP_OSK\n");
                    _osk = data->instance;
                    _osk->setView(new AppView(this, data->instance, this));
                    _osk->view()->setGeometry(0, height(), width(), 400);
                    addWidget(_osk->view());
                    _statusBar->view()->bringToFront();
                    _osk->view()->setZ(0);
                    _osk->view()->addWindow(dfbWindow);
                } else if (appInfo->appFlags() & APP_HOME)
                {
                    ILOG_DEBUG(ILX_COMPOSITOR, " -> APP_HOME\n");
                    _home = data->instance;
                    _home->setView(new AppView(this, _home, this));
                    _home->view()->setGeometry(0, 0, width(), height() - 50);
                    addWidget(_home->view());
                    _home->view()->setZ(0);
                    _home->view()->sendToBack();
                    _home->view()->addWindow(dfbWindow);
                } else if (appInfo->appFlags() & APP_SYSTEM)
                {
                    ILOG_DEBUG(ILX_COMPOSITOR, " -> APP_SYSTEM\n");

                    if (appInfo->name() == "Dashboard")
                        _dash = data->instance;
                    else if (appInfo->name() == "SoundMixer")
                        _mixer = data->instance;

                    data->instance->setView(
                            new AppView(this, data->instance, this));
                    data->instance->view()->setGeometry(0, 0, width(),
                                                        height() - 50);
                    addWidget(data->instance->view());
                    data->instance->view()->setZ(0);
                    data->instance->view()->sendToBack();
                    data->instance->view()->addWindow(dfbWindow);
                } else
                {
                    ILOG_DEBUG(ILX_COMPOSITOR, " -> APP_DEFAULT\n");
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

                    data->instance->thumb()->addWindow(dfbWindow, false);
                    if (data->instance->thumb()->_close)
                        data->instance->thumb()->_close->bringToFront();
                    data->instance->view()->addWindow(
                            dfbWindow, true,
                            !(appInfo->appFlags() & APP_SURFACE_DONTBLOCK));
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
                ILOG_DEBUG(
                        ILX_COMPOSITOR,
                        "CET_Quit (%s)\n", data->instance->appInfo()->name().c_str());

                if (data->instance == _currentApp)
                    _currentApp = NULL;

                if (data->instance->view())
                    removeWidget(data->instance->view());

                if (data->instance->thumb())
                    _switcher->removeThumb(data->instance->thumb());

                delete data->instance;

                if (_currentApp == NULL)
                    toggleLauncher(true);
            }
            break;

        case CET_Term:
            {
                ILOG_DEBUG(
                        ILX_COMPOSITOR,
                        "CET_Term (%s)\n", data->instance->appInfo()->name().c_str());

                if (data->instance->view())
                    removeWidget(data->instance->view());

                if (data->instance->thumb())
                    _switcher->removeThumb(data->instance->thumb());

                _appMan->stopApplication(data->instance->pid());
            }
            break;

        default:
            break;
        }

        delete data;
    }
}

bool
ILXCompositor::windowPreEventFilter(const DFBWindowEvent& event)
{
    switch (event.type)
    {
    case DWET_KEYDOWN:
        if (event.key_symbol == DIKS_HOME)
        {
            toggleLauncher(true); // show launcher
            return true;
        } else if (event.key_symbol == DIKS_TAB && event.modifiers == DIMM_ALT)
        {
            if (_appMan->instanceCount() < 2)
                return true;

            if (!_switcher->visible())
                toggleSwitcher(true);

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
        else if (event.key_symbol == DIKS_F12)
            quit();
        break;

    case DWET_KEYUP:
        {
            if (event.key_symbol == DIKS_ALT)
            {
                if (_appMan->instanceCount() < 2)
                    return true;

                if (_switcher->visible())
                    toggleSwitcher(false);
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
        ilixi::ILXCompositor app(argc, argv);
        app.exec();
    } catch (const std::exception& e)
    {
        ILOG_FATAL(ilixi::ILX_COMPOSITOR, "Exception: %s\n", e.what());
    }
    return 0;
}
