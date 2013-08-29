/*
 Copyright 2010-2013 Tarik Sekmen.

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

#include <compositor/Compositor.h>

#include <compositor/Notification.h>
#include <core/Engine.h>
#include <core/Logger.h>
#include <core/PlatformManager.h>
#include <lib/Notify.h>
#include <lib/XMLReader.h>
#include <ui/Label.h>

#include <sigc++/bind.h>
#include <sstream>

using namespace std;

namespace ilixi
{
D_DEBUG_DOMAIN( ILX_COMPOSITOR, "ilixi/compositor/Compositor", "Compositor");

ILXCompositor::ILXCompositor(int argc, char* argv[])
        : Application(&argc, &argv, (AppOptions) (OptExclusive | OptDaleAuto | OptSound)),
          _appMan(NULL),
          _currentApp(NULL),
          _previousApp(NULL),
          _switcher(NULL),
          _fps(NULL),
          _compComp(NULL),
          _soundComp(NULL),
          _oskComp(NULL),
          _home(NULL),
          _statusBar(NULL),
          _osk(NULL),
          _oskTargetPID(0)
{
    setenv("WEBKIT_IGNORE_SSL_ERRORS", "1", 0);
    setenv("LITE_WINDOW_DOUBLEBUFFER", "1", 0);
    setenv("LITE_NO_THEME", "1", 0);

    parseSettings();
    _soundComp = new SoundComponent();
    _compComp = new CompositorComponent(this);
    _oskComp = new OSKComponent(this);

    _appMan = new ApplicationManager(this);

    setMargin(0);

    appWindow()->setBackgroundClear(false);
    _syncWithSurfaceEvents = true;

    _fps = new FPSCalculator();

    sigVisible.connect(sigc::mem_fun(this, &ILXCompositor::onVisible));
}

ILXCompositor::~ILXCompositor()
{
    ILOG_TRACE_F(ILX_COMPOSITOR);
    delete _appMan;
    delete _fps;
    delete _compComp;
    delete _soundComp;
    delete _oskComp;
}

void
ILXCompositor::setSwitcher(Switcher* switcher)
{
    removeWidget(_switcher);
    _switcher = switcher;
    _switcher->sigSwitchRequest.connect(sigc::mem_fun1(this, &ILXCompositor::showInstance));
    _switcher->sigFeedbackRequest.connect(sigc::mem_fun(_compComp, &CompositorComponent::notifyVisibility));
    addWidget(_switcher);
}

ApplicationManager*
ILXCompositor::appMan() const
{
    return _appMan;
}

void
ILXCompositor::showInstance(AppInstance* instance)
{
    ILOG_TRACE_F(ILX_COMPOSITOR);
    toggleSwitcher(false);
    if (!instance || !instance->view())
    {
        toggleLauncher(true);
        return;
    } else if (instance == _currentApp)
        return;

    _previousApp = _currentApp;
    _currentApp = instance;

    ILOG_DEBUG(ILX_COMPOSITOR, " -> instance: %s\n", instance->appInfo()->name().c_str());

    // osk & _previousApp
    if (_osk && _osk->view()->visible())
    {
        _osk->view()->hide(AppView::Position, _oskGeometry.x(), height());
        if (_previousApp)
            _previousApp->view()->hide(AppView::AnimatedProperty(settings.hideAnimProps | AppView::Position), _appGeometry.x(), _appGeometry.y());
    } else if (_previousApp)
        _previousApp->view()->hide(settings.hideAnimProps);

    // background
    AppInfo* info = _currentApp->appInfo();

    ILOG_INFO(ILX_COMPOSITOR, "NOW SHOWING: %s\n", info->name().c_str());
    _currentApp->view()->show(settings.showAnimProps);
    _compComp->signalInstanceChanged(_currentApp, _previousApp);

    if (info->appFlags() & APP_USE_BACK)
        _compComp->signalBack(true);
    else
        _compComp->signalBack(false);
    _currentApp->view()->setWindowFocus();

    disableSurfaceEventSync(500000);
}

void
ILXCompositor::toggleLauncher(bool show)
{
    ILOG_TRACE_F(ILX_COMPOSITOR);
    if (show && _home)
        showInstance(_home);
    else if (_previousApp)
        showInstance(_previousApp);
}

void
ILXCompositor::toggleSwitcher(bool show)
{
    ILOG_TRACE_F(ILX_COMPOSITOR);
    if (!_switcher)
        return;
    if (show && _switcher->itemCount() > 0)
    {
        _switcher->show();
        _compComp->signalSwitcher(true);
        appWindow()->eventManager()->setGrabbedWidget(NULL);
        toggleOSK(false);
        if (_switcher->currentThumb())
            _switcher->currentThumb()->setFocus();
    } else
    {
        appWindow()->setBackgroundClear(true);
        _switcher->hide();
        _compComp->signalSwitcher(false);
        if (_currentApp)
            _currentApp->view()->setWindowFocus();
    }

    disableSurfaceEventSync(500000);
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

    Engine::instance().postUserEvent(CET_Term, data);
}

const Rectangle&
ILXCompositor::getAppGeometry() const
{
    return _appGeometry;
}

const Rectangle&
ILXCompositor::getBarGeometry() const
{
    return _barGeometry;
}

const Rectangle&
ILXCompositor::getOskGeometry() const
{
    return _oskGeometry;
}

const Rectangle&
ILXCompositor::getSwitcherGeometry() const
{
    return _switcherGeometry;
}

void
ILXCompositor::showOSK(DFBRectangle rect, pid_t process)
{
    ILOG_TRACE_F(ILX_COMPOSITOR);
    ILOG_DEBUG(ILX_COMPOSITOR, " -> process: %d\n", process);
    if (rect.y + rect.h > _appGeometry.bottom() - _oskGeometry.height())
        _oskTargetRect.setRectangle(rect.x, rect.y + rect.h - (_appGeometry.bottom() - _oskGeometry.height()), rect.w, rect.h);
    else
        _oskTargetRect.setRectangle(rect.x, 0, rect.w, rect.h);

    _oskTargetPID = process;
    if (!_osk)
        _appMan->startApplication("OnScreenKeyboard");
    else
        toggleOSK(true);
}

void
ILXCompositor::toggleOSK(bool show)
{
    if (_osk)
    {
        ILOG_TRACE_F(ILX_COMPOSITOR);

        if (_currentApp && _currentApp->pid() == _oskTargetPID)
        {
            if (show)
            {
                _currentApp->view()->slideTo(_appGeometry.x(), -_oskTargetRect.y());
                _osk->view()->show(AppView::Position, _oskGeometry.x(), _appGeometry.bottom() - _oskGeometry.height());
                toggleSwitcher(false);
                _osk->view()->setWindowFocus();
            } else
            {
                _currentApp->view()->slideTo(_appGeometry.x(), _appGeometry.y());
                _osk->view()->hide(AppView::Position, _oskGeometry.x(), _oskGeometry.y());
                _oskTargetPID = 0;
            }
        }
    }

    disableSurfaceEventSync(500000);
}

void
ILXCompositor::sendOSKInput(uint32_t key, unsigned int mask)
{
    if (mask == DIMM_SHIFT)
    {
        _currentApp->view()->consumeKeyEvent(KeyEvent(KeyDownEvent, DIKS_SHIFT, DIMM_SHIFT));
        _currentApp->view()->consumeKeyEvent(KeyEvent(KeyDownEvent, (DFBInputDeviceKeySymbol) key));
        _currentApp->view()->consumeKeyEvent(KeyEvent(KeyUpEvent, (DFBInputDeviceKeySymbol) key));
        _currentApp->view()->consumeKeyEvent(KeyEvent(KeyUpEvent, DIKS_SHIFT, DIMM_SHIFT));
    } else
    {
        KeyEvent k = KeyEvent(KeyDownEvent, (DFBInputDeviceKeySymbol) key);
        k.modifierMask = (DFBInputDeviceModifierMask) mask;
        _currentApp->view()->consumeKeyEvent(k);
        k.eventType = KeyUpEvent;
        _currentApp->view()->consumeKeyEvent(k);
    }

    if (key == DIKS_ENTER)
        toggleOSK(false);
}

void
ILXCompositor::setLayerOpacity(u8 opacity)
{
    ILOG_TRACE_F(ILX_COMPOSITOR);
    ILOG_DEBUG(ILX_COMPOSITOR, " -> %d\n", opacity);
    IDirectFBDisplayLayer* layer = PlatformManager::instance().getLayer("ui");
    layer->SetOpacity(layer, opacity);
}

void
ILXCompositor::compose(const PaintEvent& event)
{
    //D_INFO_LINE_MSG( "SURFEVT: compose %4d,%4d-%4dx%4d", event.rect.x(), event.rect.y(), event.rect.width(), event.rect.height() );

    _fps->funck();
}

void
ILXCompositor::setOSKGeometry(const Rectangle& rect)
{
    _oskGeometry = rect;
}

void
ILXCompositor::setAppGeometry(const Rectangle& rect)
{
    _appGeometry = rect;
}

void
ILXCompositor::setBarGeometry(const Rectangle& rect)
{
    _barGeometry = rect;
}

void
ILXCompositor::setSwitcherGeometry(const Rectangle& rect)
{
    _switcherGeometry = rect;
}

void
ILXCompositor::onVisible()
{
    if (_switcher)
        _switcher->setSwitcherGeometry(_switcherGeometry);

    _appMan->initStartup();
}

IDirectFBWindow*
ILXCompositor::getWindow(DFBWindowID wid)
{
    ILOG_TRACE_F(ILX_COMPOSITOR);
    if (wid)
    {
        ILOG_DEBUG(ILX_COMPOSITOR, " -> DFBWindowID: %u\n", wid);
        IDirectFBDisplayLayer* layer;
        DFBResult ret = PlatformManager::instance().getDFB()->GetDisplayLayer(PlatformManager::instance().getDFB(), PlatformManager::instance().getLayerID("ui"), &layer);
        if (ret)
        {
            ILOG_ERROR( ILX_COMPOSITOR, "Error! GetDisplayLayer: %s\n", DirectFBErrorString(ret));
            return NULL;
        }

        IDirectFBWindow* window;
        ret = layer->GetWindow(layer, wid, &window);
        if (ret)
        {
            layer->Release(layer);
            ILOG_ERROR( ILX_COMPOSITOR, "Error! GetWindow: %s\n", DirectFBErrorString(ret));
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
    ILOG_TRACE_F(ILX_COMPOSITOR);
    ILOG_DEBUG(ILX_COMPOSITOR, " -> ID %u\n", info->win_id);

    CompositorEventData* data = new CompositorEventData;
    data->instance = instance;
    data->windowID = info->win_id;

    Engine::instance().postUserEvent(CET_Add, data);
}

void
ILXCompositor::removeWindow(AppInstance* instance, const SaWManWindowInfo* info)
{
    ILOG_TRACE_F(ILX_COMPOSITOR);
    ILOG_DEBUG(ILX_COMPOSITOR, " -> ID %u\n", info->win_id);

    CompositorEventData* data = new CompositorEventData;
    data->instance = instance;
    data->windowID = info->win_id;

    Engine::instance().postUserEvent(CET_Remove, data);
}

void
ILXCompositor::configWindow(AppInstance* instance, SaWManWindowReconfig *reconfig, const SaWManWindowInfo* info)
{
    ILOG_TRACE_F(ILX_COMPOSITOR);
    ILOG_DEBUG(ILX_COMPOSITOR, " -> ID %u\n", info->win_id);

    CompositorEventData* data = new CompositorEventData;
    data->instance = instance;
    data->reconfig = new SaWManWindowReconfig;
    data->reconfig->caps = reconfig->caps;
    data->reconfig->current = reconfig->current;
    data->reconfig->flags = reconfig->flags;
    data->reconfig->handle = reconfig->handle;
    data->reconfig->request = reconfig->request;
    data->windowID = info->win_id;
    usleep(10000);
    Engine::instance().postUserEvent(CET_Config, data);
}

void
ILXCompositor::restackWindow(AppInstance* instance, const SaWManWindowInfo* info, int order, DFBWindowID other)
{
    ILOG_TRACE_F(ILX_COMPOSITOR);
    ILOG_DEBUG(ILX_COMPOSITOR, " -> ID %u\n", info->win_id);

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

    Engine::instance().postUserEvent(CET_Restack, data);
}

void
ILXCompositor::stateWindow(DFBWindowID wid, const DFBWindowState* state)
{
    ILOG_TRACE_F(ILX_COMPOSITOR);
    ILOG_DEBUG(ILX_COMPOSITOR, " -> ID %u\n", wid);
}

void
ILXCompositor::focusWindow(DFBWindowID wid)
{
    ILOG_TRACE_F(ILX_COMPOSITOR);
    ILOG_DEBUG(ILX_COMPOSITOR, " -> ID %u\n", wid);
}

void
ILXCompositor::processRemoved(AppInstance* instance)
{
    ILOG_TRACE_F(ILX_COMPOSITOR);
    ILOG_DEBUG(ILX_COMPOSITOR, " -> PID %u\n", instance->pid());

    CompositorEventData* data = new CompositorEventData;
    data->instance = instance;

    Engine::instance().postUserEvent(CET_Quit, data);
}

void
ILXCompositor::processTerminated(AppInstance* instance)
{
    ILOG_TRACE_F(ILX_COMPOSITOR);
    ILOG_DEBUG(ILX_COMPOSITOR, " -> PID %u\n", instance->pid());

    CompositorEventData* data = new CompositorEventData;
    data->instance = instance;

    Engine::instance().postUserEvent(CET_Crash, data);
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
                    _statusBar->setView(new AppView(this, data->instance));
                    _statusBar->view()->setGeometry(_barGeometry);
                    addWidget(_statusBar->view());
                    _statusBar->view()->setZ(0);
                    _statusBar->view()->bringToFront();
                    _statusBar->view()->addWindow(dfbWindow);
                } else if (appInfo->appFlags() & APP_OSK)
                {
                    ILOG_DEBUG(ILX_COMPOSITOR, " -> APP_OSK\n");

                    _osk = data->instance;
                    if (!_osk->view())
                    {
                        _osk->setView(new AppView(this, data->instance));
                        _osk->view()->setGeometry(_oskGeometry);
                        addWidget(_osk->view());
                        if (_statusBar)
                            _statusBar->view()->bringToFront();
                        _osk->view()->setZ(0);
                    }
                    _osk->view()->addWindow(dfbWindow, true, false);
                } else if (appInfo->appFlags() & APP_HOME)
                {
                    ILOG_DEBUG(ILX_COMPOSITOR, " -> APP_HOME\n");

                    _home = data->instance;
                    _home->setView(new AppView(this, _home));
                    data->instance->view()->setGeometry(_appGeometry);
                    addWidget(_home->view());
                    _home->view()->setZ(0);
                    _home->view()->sendToBack();
                    _home->view()->addWindow(dfbWindow);
                } else if (appInfo->appFlags() & APP_SYSTEM)
                {
                    ILOG_DEBUG(ILX_COMPOSITOR, " -> APP_SYSTEM\n");

                    if (data->instance->view() == NULL)
                    {
                        data->instance->setView(new AppView(this, data->instance));
                        data->instance->view()->setGeometry(_appGeometry);
                        addWidget(data->instance->view());
                        data->instance->view()->setZ(0);
                        data->instance->view()->sendToBack();
                    }
                    data->instance->view()->addWindow(dfbWindow, true, !(appInfo->appFlags() & APP_SURFACE_DONTBLOCK));
                } else
                {
                    ILOG_DEBUG(ILX_COMPOSITOR, " -> APP_DEFAULT\n");

                    if (data->instance->thumb() == NULL)
                    {
                        data->instance->setView(new AppView(this, data->instance));
                        data->instance->view()->setGeometry(_appGeometry);
                        data->instance->view()->setZ(-5);
                        addWidget(data->instance->view());
                        data->instance->view()->sendToBack();
                        if (_switcher)
                        {
                            data->instance->setThumb(new AppThumbnail(this, data->instance));
                            _switcher->addThumb(data->instance->thumb());
                            data->instance->view()->setNeighbour(Down, _switcher);
                        }
                    }

                    data->instance->view()->addWindow(dfbWindow, true, !(appInfo->appFlags() & APP_SURFACE_DONTBLOCK));
                    data->instance->thumb()->addWindow(dfbWindow, false);
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
                    data->instance->view()->onWindowConfig(data->windowID, data->reconfig);
                if (data->instance->thumb())
                    data->instance->thumb()->onWindowConfig(data->windowID, data->reconfig);

                delete data->reconfig;
            }
            break;

        case CET_Focus:
            break;

        case CET_Restack:
            {
                ILOG_DEBUG(ILX_COMPOSITOR, "CET_Restack (%d)\n", data->windowID);

                if (data->instance->view())
                    data->instance->view()->onWindowConfig(data->windowID, data->reconfig);
                if (data->instance->thumb())
                    data->instance->thumb()->onWindowConfig(data->windowID, data->reconfig);

                delete data->reconfig;
            }
            break;

        case CET_State:
            break;

        case CET_Quit:
            {
                ILOG_DEBUG( ILX_COMPOSITOR, "CET_Quit (%s)\n", data->instance->appInfo()->name().c_str());

                _compComp->signalAppQuit(data->instance);

                if (data->instance == _currentApp)
                    _currentApp = NULL;

                if (data->instance->view())
                    removeWidget(data->instance->view());

                if (_switcher && data->instance->thumb())
                    _switcher->removeThumb(data->instance->thumb());

                delete data->instance;

                if (_currentApp == NULL)
                    toggleLauncher(true);
            }
            break;

        case CET_Term:
            {
                ILOG_DEBUG( ILX_COMPOSITOR, "CET_Term (%s)\n", data->instance->appInfo()->name().c_str());

                _compComp->signalAppQuit(data->instance);

                if (data->instance->view())
                    removeWidget(data->instance->view());

                if (_switcher && data->instance->thumb())
                    _switcher->removeThumb(data->instance->thumb());

                _appMan->stopApplication(data->instance->pid());
            }
            break;

        case CET_Crash:
            {
                ILOG_DEBUG( ILX_COMPOSITOR, "CET_Crash (%s)\n", data->instance->appInfo()->name().c_str());

                _compComp->signalAppQuit(data->instance);

                std::stringstream ss;
                ss << data->instance->appInfo()->name() << " terminated abnormally.";
                Notify notify("Application crashed!", ss.str());
                notify.setIcon(ILIXI_DATADIR"images/default.png");
                notify.show();

                if (data->instance == _currentApp)
                    _currentApp = NULL;

                if (data->instance->view())
                    removeWidget(data->instance->view());

                if (_switcher && data->instance->thumb())
                    _switcher->removeThumb(data->instance->thumb());

                delete data->instance;
                toggleLauncher(true);
            }
            break;

        default:
            break;
        }

        delete data;
    }
}

bool
ILXCompositor::windowCustomEventFilter(const DFBWindowEvent& event)
{
    return false;
}

bool
ILXCompositor::windowPreEventFilter(const DFBWindowEvent& event)
{
    if (windowCustomEventFilter(event))
        return true;

    switch (event.type)
    {
    case DWET_KEYDOWN:
        switch (event.key_symbol)
        {
        case DIKS_HOME:
            if (_home->view()->visible())
                toggleLauncher(false); // show current application
            else
                toggleLauncher(true); // show launcher
            return true;

        case DIKS_PAUSE:
            if (_osk && _osk->view()->visible())
                toggleOSK(false); // show current application
            else if (_currentApp)
            {
                if (1 || !_osk)
                {
                    DFBRectangle rect = { 0, 0, 800, 30 };
                    showOSK(rect, _currentApp->pid());
                } else
                    toggleOSK(true); // show osk
            }
            return true;

        case DIKS_TAB:
            if (event.modifiers == DIMM_ALT)
            {
                if (_appMan->instanceCount() < 2)
                    return true;

                if (_switcher)
                {
                    if (!_switcher->visible())
                        toggleSwitcher(true);
                    _switcher->scrollToNext();
                }
                return true;
            }
            return appWindow()->eventManager()->focusedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, (DFBInputDeviceKeySymbol) DIKS_TAB));

        case DIKS_ESCAPE:
            return true;

        case DIKS_OK:
            if (_switcher && _switcher->visible())
            {
                _switcher->sigSwitchRequest(_switcher->currentThumb()->instance());
                return true;
            } else
                return appWindow()->eventManager()->focusedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, (DFBInputDeviceKeySymbol) DIKS_OK));

        case DIKS_SPACE:
            if (_switcher && _switcher->visible())
            {
                _switcher->sigSwitchRequest(_switcher->currentThumb()->instance());
                return true;
            } else
                return false;

        case DIKS_RETURN:
            if (appWindow()->eventManager()->focusedWidget())
                return appWindow()->eventManager()->focusedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, (DFBInputDeviceKeySymbol) DIKS_RETURN));
            break;

        case DIKS_CURSOR_UP:
            if (appWindow()->eventManager()->focusedWidget())
                return appWindow()->eventManager()->focusedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, (DFBInputDeviceKeySymbol) DIKS_CURSOR_UP));
            break;

        case DIKS_CURSOR_DOWN:
            if (appWindow()->eventManager()->focusedWidget())
                return appWindow()->eventManager()->focusedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, (DFBInputDeviceKeySymbol) DIKS_CURSOR_DOWN));
            break;

        case DIKS_CURSOR_LEFT:
            if (_switcher && _switcher->visible())
            {
                _switcher->scrollToNext();
                return true;
            } else if (appWindow()->eventManager()->focusedWidget())
                return appWindow()->eventManager()->focusedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, (DFBInputDeviceKeySymbol) DIKS_CURSOR_LEFT));
            break;

        case DIKS_CURSOR_RIGHT:
            if (_switcher && _switcher->visible())
            {
                _switcher->scrollToPrevious();
                return true;
            } else if (appWindow()->eventManager()->focusedWidget())
                return appWindow()->eventManager()->focusedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, (DFBInputDeviceKeySymbol) DIKS_CURSOR_RIGHT));
            break;

        case DIKS_F11:
            {
                Notify notify("Hi there!", "This is a notification...");
                notify.setIcon(ILIXI_DATADIR"images/default.png");
                notify.show();
                return true;
            }

        case DIKS_F12:
            quit();
            return true;

        default:
            break;
        }
        break;

    case DWET_KEYUP:
        {
            if (event.key_symbol == DIKS_ALT)
            {
                if (_appMan->instanceCount() < 2)
                    return true;

                if (_switcher->visible())
                    toggleSwitcher(false);

                _switcher->sigSwitchRequest(_switcher->currentThumb()->instance());
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
ILXCompositor::parseSettings()
{
    XMLReader xml;
    if (xml.loadFile((ILIXI_DATADIR"compositor.xml")) == false)
    {
        ILOG_FATAL(ILX_COMPOSITOR, "Could not parse compositor settings!\n");
        return false;
    }

    xmlNodePtr group = xml.currentNode();
    xmlNodePtr element;
    while (group != NULL)
    {
        ILOG_DEBUG(ILX_COMPOSITOR, " -> group: %s\n", (char*)group->name);
        if (xmlStrcmp(group->name, (xmlChar*) "mem_monitor") == 0)
        {
            xmlChar* enabled = xmlGetProp(group, (xmlChar*) "enabled");
            if (xmlStrcmp(enabled, (xmlChar*) "yes") == 0)
                settings.memMonitor = true;
            else
                settings.memMonitor = false;

            element = group->children;
            while (element != NULL)
            {
                ILOG_DEBUG(ILX_COMPOSITOR, "   -> element: %s\n", (char*)element->name);
                if (xmlStrcmp(element->name, (xmlChar*) "mem_states") == 0)
                {
                    xmlChar* low = xmlNodeGetContent(element->children);
                    xmlChar* crit = xmlNodeGetContent(element->children->next);
                    settings.memLow = atof((char*) low);
                    settings.memCritical = atof((char*) crit);
                    ILOG_DEBUG(ILX_COMPOSITOR, "    -> memLow: %f\n", settings.memLow);
                    ILOG_DEBUG(ILX_COMPOSITOR, "    -> memCritical: %f\n", settings.memCritical);
                    xmlFree(low);
                    xmlFree(crit);
                } else if (xmlStrcmp(element->name, (xmlChar*) "page_faults") == 0)
                {
                    xmlChar* low = xmlNodeGetContent(element->children);
                    xmlChar* crit = xmlNodeGetContent(element->children->next);
                    settings.pgLow = atoi((char*) low);
                    settings.pgCritical = atoi((char*) crit);
                    ILOG_DEBUG(ILX_COMPOSITOR, "    -> pgLow: %d\n", settings.pgLow);
                    ILOG_DEBUG(ILX_COMPOSITOR, "    -> pgCritical: %d\n", settings.pgCritical);
                    xmlFree(low);
                    xmlFree(crit);
                }
                element = element->next;
            }
        } else if (xmlStrcmp(group->name, (xmlChar*) "animations") == 0)
        {
            element = group->children;
            xmlChar* enabled = xmlGetProp(group, (xmlChar*) "enabled");
            if (xmlStrcmp(enabled, (xmlChar*) "yes") == 0)
            {
                settings.animations = true;
                while (element != NULL)
                {
                    ILOG_DEBUG(ILX_COMPOSITOR, "   -> element: %s\n", (char*)element->name);
                    if (xmlStrcmp(element->name, (xmlChar*) "slide") == 0)
                    {
                        xmlChar* duration = xmlNodeGetContent(element->children);
                        settings.durationSlide = atoi((char*) duration);

                        ILOG_DEBUG(ILX_COMPOSITOR, "    -> duration: %s\n", (char*) duration);

                        xmlFree(duration);
                    } else if (xmlStrcmp(element->name, (xmlChar*) "show") == 0)
                    {
                        xmlChar* zoom = xmlNodeGetContent(element->children);
                        xmlChar* opacity = xmlNodeGetContent(element->children->next);
                        xmlChar* duration = xmlNodeGetContent(element->children->next->next);

                        if (atoi((char*) zoom))
                            settings.showAnimProps = AppView::AnimatedProperty(settings.showAnimProps | AppView::Zoom);
                        if (atoi((char*) opacity))
                            settings.showAnimProps = AppView::AnimatedProperty(settings.showAnimProps | AppView::Opacity);
                        settings.durationShow = atoi((char*) duration);

                        ILOG_DEBUG(ILX_COMPOSITOR, "    -> zoom: %s opacity: %s duration: %s\n", (char*) zoom, (char*) opacity, (char*) duration);
                        ILOG_DEBUG(ILX_COMPOSITOR, "    -> AnimProps: %x\n", settings.showAnimProps);

                        xmlFree(zoom);
                        xmlFree(opacity);
                        xmlFree(duration);
                    } else if (xmlStrcmp(element->name, (xmlChar*) "hide") == 0)
                    {
                        xmlChar* zoom = xmlNodeGetContent(element->children);
                        xmlChar* opacity = xmlNodeGetContent(element->children->next);
                        xmlChar* duration = xmlNodeGetContent(element->children->next->next);

                        if (atoi((char*) zoom))
                            settings.hideAnimProps = AppView::AnimatedProperty(settings.hideAnimProps | AppView::Zoom);
                        if (atoi((char*) opacity))
                            settings.hideAnimProps = AppView::AnimatedProperty(settings.hideAnimProps | AppView::Opacity);
                        settings.durationHide = atoi((char*) duration);

                        ILOG_DEBUG(ILX_COMPOSITOR, "    -> zoom: %s opacity: %s duration: %s\n", (char*) zoom, (char*) opacity, (char*) duration);
                        ILOG_DEBUG(ILX_COMPOSITOR, "    -> AnimProps: %x\n", settings.hideAnimProps);

                        xmlFree(zoom);
                        xmlFree(opacity);
                        xmlFree(duration);
                    }
                    element = element->next;
                }
            } else
            {
                settings.animations = false;
                settings.durationSlide = 0;
                settings.showAnimProps = AppView::None;
                settings.durationShow = 0;
                settings.hideAnimProps = AppView::None;
                settings.durationHide = 0;
            }
            xmlFree(enabled);
        } else if (xmlStrcmp(group->name, (xmlChar*) "notifications") == 0)
        {
            element = group->children;
            xmlChar* duration = xmlNodeGetContent(element);
            settings.notificationTimeout = atoi((char*) duration);
            xmlFree(duration);
        }
        group = group->next;
    }

    ILOG_INFO(ILX_COMPOSITOR, "Parsed compositor settings.\n");
    return true;
}

void
ILXCompositor::appVisible()
{
    if (_currentApp)
    {
        AppInfo* info = _currentApp->appInfo();
        if (info->appFlags() & APP_NEEDS_CLEAR)
            appWindow()->setBackgroundClear(true);
        else
            appWindow()->setBackgroundClear(false);
    }
}

} /* namespace ilixi */

