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

#include <core/AppBase.h>
#include <ui/WindowWidget.h>
#include <lib/Timer.h>
#include <core/Logger.h>
#include <algorithm>
#include <stdlib.h>
#include <limits.h>
#include <directfb_util.h>
#if ILIXI_HAVE_FUSIONDALE
#include <core/DaleDFB.h>
#endif
#if ILIXI_HAVE_FUSIONSOUND
#include <core/SoundDFB.h>
#endif

#include <core/PlatformManager.h>

using namespace std;

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_APPBASE, "ilixi/core/AppBase", "AppBase");
D_DEBUG_DOMAIN( ILX_APPBASE_UPDATES, "ilixi/core/AppBase/Updates", "AppBase Updates");

IDirectFBEventBuffer* AppBase::__buffer = NULL;
AppBase* AppBase::__instance = NULL;

AppBase::AppBase(int* argc, char*** argv, AppOptions options)
        : __flags(APS_HIDDEN),
          __activeWindow(NULL),
          _update(false),
          _updateFromSurfaceView(false),
          _updateID(0),
          _updateFlipCount(0),
          _updateDiff(0),
          _updateTime(0),
          _updateDisable(0),
          _syncWithSurfaceEvents(false)
{
    if (__instance)
        ILOG_THROW(ILX_APPBASE, "Cannot allow more than one instance!\n");

    __instance = this;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&__cbMutex, &attr);
#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
    pthread_mutex_init(&__selMutex, NULL);
#endif
    pthread_mutex_init(&__windowMutex, &attr);
    pthread_mutex_init(&__timerMutex, &attr);

    setenv("XML_CATALOG_FILES", ILIXI_DATADIR"ilixi_catalog.xml", 0);

    __cursorNew.x = 0;
    __cursorNew.y = 0;

    __cursorOld.x = 0;
    __cursorOld.y = 0;

    PlatformManager::instance().initialize(argc, argv, options);
    initEventBuffer();
    Size s = PlatformManager::instance().getLayerSize("ui");
    __layerSize.w = s.width() - 1;
    __layerSize.h = s.height() - 1;

    _update_timer = new Timer();
    _update_timer->sigExec.connect(sigc::mem_fun(this, &AppBase::updateTimeout));
    _update_timer->setInterval(100);
    _update_timer->setRepeats(1);
}

AppBase::~AppBase()
{
    ILOG_TRACE_F(ILX_APPBASE);
    delete _update_timer;
    releaseEventBuffer();
    PlatformManager::instance().release();

    pthread_mutex_destroy(&__cbMutex);
#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
    pthread_mutex_destroy(&__selMutex);
#endif
    pthread_mutex_destroy(&__windowMutex);
    pthread_mutex_destroy(&__timerMutex);
    __activeWindow = NULL;
}

void
AppBase::handleUserEvent(const DFBUserEvent& event)
{
}

bool
AppBase::windowPreEventFilter(const DFBWindowEvent& event)
{
    return false;
}

AppBase::AppFlags
AppBase::appFlags() const
{
    return __flags;
}

void
AppBase::setAppFlag(AppFlags state)
{
    __flags = (AppFlags) (__flags | state);
}

void
AppBase::clearAppFlags(AppFlags state)
{
    __flags = (AppFlags) (__flags & ~state);
}

void
AppBase::initEventBuffer()
{
    ILOG_TRACE_F(ILX_APPBASE);
    if (PlatformManager::instance().appOptions() & OptExclusive)
    {
        if (PlatformManager::instance().getDFB()->CreateInputEventBuffer(PlatformManager::instance().getDFB(), DICAPS_ALL, DFB_TRUE, &__buffer) != DFB_OK)
            ILOG_THROW(ILX_APPBASE, "Error while creating input event buffer!\n");
    } else if (PlatformManager::instance().getDFB()->CreateEventBuffer(PlatformManager::instance().getDFB(), &__buffer) != DFB_OK)
        ILOG_THROW(ILX_APPBASE, "Error while creating event buffer!\n");
}

void
AppBase::releaseEventBuffer()
{
    if (__buffer)
    {
        ILOG_DEBUG(ILX_APPBASE, "Releasing event buffer.\n");
        __buffer->Release(__buffer);
        __buffer = NULL;
    }
}

IDirectFBEventBuffer*
AppBase::getEventBuffer()
{
    return __buffer;
}

void
AppBase::postUniversalEvent(Widget* target, unsigned int type, void* data)
{
    UniversalEvent event(target, type, data);
    __buffer->PostEvent(__buffer, DFB_EVENT(&event) );
}

void
AppBase::postKeyEvent(DFBInputDeviceKeySymbol symbol, DFBInputDeviceModifierMask modifierMask, DFBInputDeviceLockState lockState, bool down)
{
    if (PlatformManager::instance().appOptions() & OptExclusive)
    {
        DFBInputEvent event;
        event.clazz = DFEC_INPUT;
        event.type = down ? DIET_KEYPRESS : DIET_KEYRELEASE;
        event.flags = (DFBInputEventFlags) (DIEF_KEYSYMBOL | DIEF_MODIFIERS | DIEF_LOCKS);
        event.key_symbol = symbol;
        event.modifiers = modifierMask;
        event.locks = lockState;
        __instance->__buffer->PostEvent(__buffer, DFB_EVENT(&event) );
    } else
    {
        DFBWindowEvent event;
        event.clazz = DFEC_WINDOW;
        event.window_id = activeWindow()->windowID();
        event.type = down ? DWET_KEYDOWN : DWET_KEYUP;
        event.flags = DWEF_NONE;
        event.key_symbol = symbol;
        event.modifiers = modifierMask;
        event.locks = lockState;
        __instance->__buffer->PostEvent(__buffer, DFB_EVENT(&event) );
    }
}

void
AppBase::postPointerEvent(PointerEventType type, PointerButton button, PointerButtonMask buttonMask, int x, int y, int cx, int cy, int step)
{
    if (PlatformManager::instance().appOptions() & OptExclusive)
    {
        DFBInputEvent event_1;
        event_1.clazz = DFEC_INPUT;
        event_1.type = DIET_AXISMOTION;
        event_1.flags = (DFBInputEventFlags) (DIEF_AXISABS | DIEF_FOLLOW);
        event_1.axis = DIAI_X;
        event_1.axisabs = x;
        event_1.max = __layerSize.w;
        event_1.min = 0;
        __instance->__buffer->PostEvent(__buffer, DFB_EVENT(&event_1) );

        DFBInputEvent event_2;
        event_2.clazz = DFEC_INPUT;
        event_2.type = DIET_AXISMOTION;
        event_2.flags = DIEF_AXISABS;
        event_2.axis = DIAI_Y;
        event_2.axisabs = y;
        event_2.max = __layerSize.h;
        event_2.min = 0;
        event_2.button = (DFBInputDeviceButtonIdentifier) button;
        event_2.buttons = (DFBInputDeviceButtonMask) buttonMask;
        __instance->__buffer->PostEvent(__buffer, DFB_EVENT(&event_2) );

        if (type == PointerButtonDown)
        {
            DFBInputEvent event_3;
            event_3.clazz = DFEC_INPUT;
            event_3.type = DIET_BUTTONPRESS;
            event_3.flags = DIEF_NONE;
            event_3.button = (DFBInputDeviceButtonIdentifier) button;
            event_3.buttons = (DFBInputDeviceButtonMask) buttonMask;
            __instance->__buffer->PostEvent(__buffer, DFB_EVENT(&event_3) );

        } else if (type == PointerButtonUp)
        {
            DFBInputEvent event_3;
            event_3.clazz = DFEC_INPUT;
            event_3.type = DIET_BUTTONRELEASE;
            event_3.flags = DIEF_NONE;
            event_3.button = (DFBInputDeviceButtonIdentifier) button;
            event_3.buttons = (DFBInputDeviceButtonMask) buttonMask;
            __instance->__buffer->PostEvent(__buffer, DFB_EVENT(&event_3) );
        } else if (type == PointerWheel)
        {
            DFBInputEvent event_3;
            event_3.clazz = DFEC_INPUT;
            event_3.type = DIET_AXISMOTION;
            event_3.flags = DIEF_AXISABS;
            event_3.axis = DIAI_Z;
            event_3.axisabs = step;
            event_3.max = __layerSize.h;
            event_3.min = 0;
            event_3.button = (DFBInputDeviceButtonIdentifier) button;
            event_3.buttons = (DFBInputDeviceButtonMask) buttonMask;
            __instance->__buffer->PostEvent(__buffer, DFB_EVENT(&event_3) );
        }
    } else
    {
        DFBWindowEvent event;
        event.clazz = DFEC_WINDOW;
        event.type = (DFBWindowEventType) type;
        event.window_id = activeWindow()->windowID();
        event.flags = DWEF_NONE;
        event.x = x;
        event.y = y;
        event.cx = cx;
        event.cy = cy;
        event.step = step;
        event.button = (DFBInputDeviceButtonIdentifier) button;
        event.buttons = (DFBInputDeviceButtonMask) buttonMask;
        __instance->__buffer->PostEvent(__buffer, DFB_EVENT(&event) );
    }
}

DFBPoint
AppBase::cursorPosition()
{
    return __instance->__cursorNew;
}

bool
AppBase::addCallback(Callback* cb)
{
    if (__instance && cb)
    {
        pthread_mutex_lock(&__instance->__cbMutex);

        CallbackList::iterator it = std::find(__instance->__callbacks.begin(), __instance->__callbacks.end(), cb);
        if (cb == *it)
        {
            pthread_mutex_unlock(&__instance->__cbMutex);
            ILOG_DEBUG(ILX_APPBASE, "Callback %p already added!\n", cb);
            return false;
        }
        __instance->__callbacks.push_back(cb);

        pthread_mutex_unlock(&__instance->__cbMutex);
        ILOG_DEBUG(ILX_APPBASE, "Callback %p is added.\n", cb);
        return true;
    }
    return false;
}

bool
AppBase::removeCallback(Callback* cb)
{
    if (__instance && cb)
    {
        pthread_mutex_lock(&__instance->__cbMutex);

        for (CallbackList::iterator it = __instance->__callbacks.begin(); it != __instance->__callbacks.end(); ++it)
        {
            if (cb == *it)
            {
                __instance->__callbacks.erase(it);
                pthread_mutex_unlock(&__instance->__cbMutex);
                ILOG_DEBUG(ILX_APPBASE, "Callback %p is removed.\n", cb);
                return true;
            }
        }

        pthread_mutex_unlock(&__instance->__cbMutex);
    }
    return false;
}

bool
AppBase::addTimer(Timer* timer)
{
    if (__instance && timer)
    {
        pthread_mutex_lock(&__instance->__timerMutex);

        TimerList::iterator it = std::find(__instance->_timers.begin(), __instance->_timers.end(), timer);
        if (it == __instance->_timers.end())
        {
            __instance->_timers.push_back(timer);

            pthread_mutex_unlock(&__instance->__timerMutex);
            ILOG_DEBUG(ILX_APPBASE, "Timer %p is added.\n", timer);
            return true;
        }

        pthread_mutex_unlock(&__instance->__timerMutex);
        ILOG_DEBUG(ILX_APPBASE, "Timer %p already added!\n", timer);
        return false;
    }
    return false;
}

bool
AppBase::removeTimer(Timer* timer)
{
    if (__instance && timer)
    {
        pthread_mutex_lock(&__instance->__timerMutex);

        TimerList::iterator it = std::find(__instance->_timers.begin(), __instance->_timers.end(), timer);
        if (it != __instance->_timers.end())
        {
            __instance->_timers.erase(it);
            pthread_mutex_unlock(&__instance->__timerMutex);
            ILOG_DEBUG(ILX_APPBASE, "Timer %p is removed.\n", timer);
            return true;
        }

        pthread_mutex_unlock(&__instance->__timerMutex);
    }
    return false;
}

int32_t
AppBase::runTimers()
{
    ILOG_TRACE_F(ILX_APPBASE);
    int32_t timeout = 1000000;

    pthread_mutex_lock(&__timerMutex);
    int64_t now = direct_clock_get_millis();

    int64_t nextTimeout;
    Timer* timer;
    TimerList::iterator it = _timers.begin();
    while (it != _timers.end())
    {
        timer = (Timer*) *it;
        nextTimeout = timer->expiry() - now;
        if (nextTimeout <= 0)
        {
            ILOG_DEBUG( ILX_APPBASE, " -> timer %p was supposed to fire at %d.%d\n", timer, (int) (timer->expiry() / 1000), (int)(timer->expiry() % 1000));
            if (!timer->funck())
            {
                if (timer != _update_timer)
                {
                    it = _timers.erase(it);
                    continue;
                }
            } else
                nextTimeout = timer->expiry() - now;
        }

        if (nextTimeout < timeout)
            timeout = nextTimeout;
        ++it;
    }

    pthread_mutex_unlock(&__timerMutex);
    if (timeout < 1)
        timeout = 1;
    else if (timeout > 10000)
        timeout = 10000;

    ILOG_DEBUG( ILX_APPBASE, " -> desired timeout %d.%d seconds\n", (int) (timeout / 1000), (int)(timeout % 1000));
    return timeout;
}

void
AppBase::runCallbacks()
{
    ILOG_TRACE_F(ILX_APPBASE);

    pthread_mutex_lock(&__cbMutex);
    std::vector<Callback*> list_copy;
    CallbackList::iterator it = __callbacks.begin();
    while (it != __callbacks.end())
    {
        list_copy.push_back(*it);
        ++it;
    }
    std::vector<Callback*>::iterator it2 = list_copy.begin();
    while (it2 != list_copy.end())
    {
        if (((Callback*) *it2)->_funck->funck() == 0)
        {
            ILOG_DEBUG( ILX_APPBASE, " -> Callback %p is removed.\n", ((Callback*) *it2));
            removeCallback(*it2);
        }
        ++it2;
    }
    pthread_mutex_unlock(&__cbMutex);
}
#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)

bool AppBase::addSurfaceEventListener(SurfaceEventListener* sel)
{
    if (__instance && sel)
    {
        pthread_mutex_lock(&__instance->__selMutex);

        SurfaceListenerList::iterator it = std::find(__instance->__selList.begin(), __instance->__selList.end(), sel);
        if (sel == *it)
        {
            pthread_mutex_unlock(&__instance->__selMutex);
            ILOG_ERROR( ILX_APPBASE, "SurfaceEventListener %p already added!\n", sel);
            return false;
        }

        ILOG_DEBUG(ILX_APPBASE, "SurfaceEventListener %p is added.\n", sel);
        // check whether to attach source surface
        bool attach = true;
        for (SurfaceListenerList::iterator it = __instance->__selList.begin(); it != __instance->__selList.end(); ++it)
        {
            if (sel->sourceSurface() == ((SurfaceEventListener*) *it)->sourceSurface())
            attach = false;
        }

        if (attach)
        {
            sel->sourceSurface()->MakeClient(sel->sourceSurface());
            sel->sourceSurface()->AttachEventBuffer(sel->sourceSurface(), __buffer);
            ILOG_DEBUG(ILX_APPBASE, " -> Surface[%p] is attached.\n", sel->sourceSurface());
        }

        __instance->__selList.push_back(sel);
        pthread_mutex_unlock(&__instance->__selMutex);
        return true;
    }
    return false;
}

bool
AppBase::removeSurfaceEventListener(SurfaceEventListener* sel)
{
    if (__instance && sel)
    {
        pthread_mutex_lock(&__instance->__selMutex);

        bool ret = false;
        IDirectFBSurface* source = sel->sourceSurface();

        for (SurfaceListenerList::iterator it = __instance->__selList.begin(); it != __instance->__selList.end(); ++it)
        {
            if (sel == *it)
            {
                __instance->__selList.erase(it);
                ILOG_DEBUG( ILX_APPBASE, "SurfaceEventListener %p is removed.\n", sel);
                ret = true;
                break;
            }
        }

        if (ret)
        {
            bool detach = true;
            for (SurfaceListenerList::iterator it = __instance->__selList.begin(); it != __instance->__selList.end();
                    ++it)
            {
                if (source == ((SurfaceEventListener*) *it)->sourceSurface())
                    detach = false;
            }
            pthread_mutex_unlock(&__instance->__selMutex);

            if (detach)
            {
                sel->sourceSurface()->DetachEventBuffer(sel->sourceSurface(), __buffer);
                ILOG_DEBUG( ILX_APPBASE, " -> Surface[%p] is detached.\n", sel->sourceSurface());
            }
            return true;
        } else
        {
            pthread_mutex_unlock(&__instance->__selMutex);
            return false;
        }
    }
    return false;
}

void
AppBase::consumeSurfaceEvent(const DFBSurfaceEvent& event)
{
    pthread_mutex_lock(&__selMutex);

    for (SurfaceListenerList::iterator it = __selList.begin(); it != __selList.end(); ++it)
        ((SurfaceEventListener*) *it)->consumeSurfaceEvent(event);

    pthread_mutex_unlock(&__selMutex);
}
#endif // ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
WindowWidget *
AppBase::activeWindow()
{
    if (__instance)
    {
        pthread_mutex_lock(&__instance->__windowMutex);
        WindowWidget* w = __instance->__activeWindow;
        pthread_mutex_unlock(&__instance->__windowMutex);
        return w;
    }
    return NULL;
}

Size
AppBase::appSize()
{
    return __instance->__windowList.front()->size();
}

void
AppBase::setActiveWindow(WindowWidget* window)
{
    if (__instance)
    {
        pthread_mutex_lock(&__instance->__windowMutex);

        if (__instance->__activeWindow)
        {
            detachDFBWindow(__instance->__activeWindow->_window);
            __instance->__activeWindow->_eventManager->setGrabbedWidget(NULL);
            __instance->__activeWindow->_eventManager->setExposedWidget(NULL);
        }

        __instance->__activeWindow = window;
        attachDFBWindow(__instance->__activeWindow->_window);

        pthread_mutex_unlock(&__instance->__windowMutex);
        ILOG_DEBUG(ILX_APPBASE, "WindowWidget %p is now active.\n", window);
    }
}

bool
AppBase::addWindow(WindowWidget* window)
{
    if (__instance && window)
    {
        pthread_mutex_lock(&__instance->__windowMutex);

        WindowList::iterator it = std::find(__instance->__windowList.begin(), __instance->__windowList.end(), window);
        if (window == *it)
        {
            pthread_mutex_unlock(&__instance->__windowMutex);
            ILOG_ERROR(ILX_APPBASE, "WindowWidget %p already added!\n", window);
            return false;
        }
        __instance->__windowList.push_back(window);

        pthread_mutex_unlock(&__instance->__windowMutex);
        ILOG_DEBUG(ILX_APPBASE, "WindowWidget %p is added.\n", window);
        return true;
    }
    return false;
}

bool
AppBase::removeWindow(WindowWidget* window)
{
    if (__instance && window)
    {
        pthread_mutex_lock(&__instance->__windowMutex);

        for (WindowList::iterator it = __instance->__windowList.begin(); it != __instance->__windowList.end(); ++it)
        {
            if (window == *it)
            {
                __instance->__windowList.erase(it);

                pthread_mutex_unlock(&__instance->__windowMutex);
                ILOG_DEBUG(ILX_APPBASE, "WindowWidget %p is removed.\n", window);
                __instance->setActiveWindow(__instance->__windowList.back());
                return true;
            }
        }
        pthread_mutex_unlock(&__instance->__windowMutex);
        ILOG_DEBUG( ILX_APPBASE, "Cannot remove WindowWidget, %p not found!\n", window);
    }
    return false;
}

void
AppBase::updateTimeout()
{
    ILOG_TRACE_F(ILX_APPBASE_UPDATES);
    _update = true;
    _updateDiff = 0;
}

void
AppBase::updateFromWindow()
{
    ILOG_TRACE_F(ILX_APPBASE_UPDATES);
    long long current_time = direct_clock_get_time(DIRECT_CLOCK_MONOTONIC);
    ILOG_DEBUG(ILX_APPBASE_UPDATES, " -> time %16lld   disable %16lld\n", current_time, _updateDisable);

    if (!_updateFromSurfaceView || current_time < _updateDisable)
    {
        _update = true;
        _updateDiff = 0;
        if (!(__flags & APS_CUSTOM))
            __buffer->WakeUp(__buffer);
    }
}

void
AppBase::updateWindows()
{
    ILOG_TRACE_F(ILX_APPBASE_UPDATES);
    if ((!_syncWithSurfaceEvents || _update) && !(PlatformManager::instance().appOptions() & OptNoUpdates))
    {
        pthread_mutex_lock(&__windowMutex);

        for (WindowList::iterator it = __windowList.begin(); it != __windowList.end(); ++it)
            ((WindowWidget*) *it)->updateWindow();

        _update = false;

        pthread_mutex_unlock(&__windowMutex);
    }
    ILOG_DEBUG(ILX_APPBASE_UPDATES, " -> finished updating windows.\n");
}

void
AppBase::handleEvents(int32_t timeout, bool forceWait)
{
    ILOG_TRACE_F(ILX_APPBASE);
    DFBEvent event;
    DFBWindowEvent lastMotion; // Used for compressing motion events.

    lastMotion.type = DWET_NONE;

    if (timeout < 1)
        ILOG_ERROR(ILX_APPBASE, "Timeout error with value %d\n", timeout);

    bool wait = true;
    if (!forceWait && (!_syncWithSurfaceEvents || _update) && timeout)
    {
        if (__callbacks.size())
            wait = false;
        else
        {
            for (WindowList::iterator it = __windowList.begin(); it != __windowList.end(); ++it)
            {
                if (((WindowWidget*) *it)->_updates._updateQueue.size())
                {
                    wait = false;
                    break;
                }
            }
        }
    }

    if (wait)
    {
        // discard window update event in buffer.
        DFBResult ret = __buffer->PeekEvent(__buffer, &event);
        if (ret == DFB_OK)
        {
            if ((event.clazz == DFEC_WINDOW) && (event.window.type == DWET_UPDATE))
                __buffer->GetEvent(__buffer, &event);
        }

        ILOG_DEBUG(ILX_APPBASE_UPDATES, " -> wait for event with timeout: %d.%d seconds\n", timeout / 1000, timeout % 1000);
        __buffer->WaitForEventWithTimeout(__buffer, timeout / 1000, timeout % 1000);
        ILOG_DEBUG(ILX_APPBASE_UPDATES, " -> got event! \n");
    }

    while (__buffer->GetEvent(__buffer, &event) == DFB_OK)
    {
        switch (event.clazz)
        {
        // If layer is exclusively used by application, we get DFEC_INPUT events.
        case DFEC_INPUT:
            switch (event.input.type)
            {
            case DIET_KEYPRESS:
                handleKeyInputEvent((const DFBInputEvent&) event, DWET_KEYDOWN);
                break;

            case DIET_KEYRELEASE:
                handleKeyInputEvent((const DFBInputEvent&) event, DWET_KEYUP);
                break;

            case DIET_BUTTONPRESS:
                handleButtonInputEvent((const DFBInputEvent&) event, DWET_BUTTONDOWN);
                break;

            case DIET_BUTTONRELEASE:
                handleButtonInputEvent((const DFBInputEvent&) event, DWET_BUTTONUP);
                break;

            case DIET_AXISMOTION:
                handleAxisMotion((const DFBInputEvent&) event);
                break;

            default:
                ILOG_WARNING(ILX_APPBASE, "Unknown input event type\n");
                break;
            }
            break;

        case DFEC_WINDOW:
            if (!(PlatformManager::instance().appOptions() & OptExclusive) && event.window.type != DWET_UPDATE)
            {
                if (event.window.type == DWET_MOTION)
                    lastMotion = event.window;
                else if (lastMotion.type == DWET_NONE)
                {
                    if (!windowPreEventFilter((const DFBWindowEvent&) lastMotion))
                        activeWindow()->handleWindowEvent((const DFBWindowEvent&) lastMotion);
                    lastMotion.type = DWET_NONE;
                    if (!windowPreEventFilter((const DFBWindowEvent&) event.window))
                        activeWindow()->handleWindowEvent((const DFBWindowEvent&) event.window);
                }
            }
            break;

        case DFEC_USER:
            handleUserEvent((const DFBUserEvent&) event);
            break;

        case DFEC_UNIVERSAL:
            {
                ILOG_DEBUG(ILX_APPBASE, "UniversalEvent\n");
                UniversalEvent* uEvent = (UniversalEvent*) &event;
                ILOG_DEBUG(ILX_APPBASE, " -> target: %p\n", uEvent->target);
                uEvent->target->universalEvent(uEvent);
                break;
            }
#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
        case DFEC_SURFACE:
            {
                ILOG_DEBUG(ILX_APPBASE_UPDATES, " -> SURFACE EVENT [%3d]  %4d,%4d-%4dx%4d (count %d)\n", event.surface.surface_id, DFB_RECTANGLE_VALS_FROM_REGION(&event.surface.update), event.surface.flip_count);
                for (SurfaceListenerList::iterator it = __selList.begin(); it != __selList.end(); ++it)
                    ((SurfaceEventListener*) *it)->consumeSurfaceEvent((const DFBSurfaceEvent&) event);
            }
            break;
#endif
        default:
            break;
        }
    }

    if (lastMotion.type != 0)
        if (!windowPreEventFilter((const DFBWindowEvent&) lastMotion))
            activeWindow()->handleWindowEvent((const DFBWindowEvent&) lastMotion);
    ILOG_DEBUG(ILX_APPBASE, " -> end handle events \n");
}

void
AppBase::accountSurfaceEvent(const DFBSurfaceEvent& event, long long lastTime)
{
    if (event.surface_id != _updateID || event.flip_count != _updateFlipCount)
    {
        ILOG_TRACE_F(ILX_APPBASE_UPDATES);
        long long diff = event.time_stamp - lastTime;

        ILOG_DEBUG(ILX_APPBASE_UPDATES, " -> account surface id %d, updateID %d, flip_count %u, updateFlipCount %u\n", event.surface_id, _updateID, event.flip_count, _updateFlipCount);
        ILOG_DEBUG(ILX_APPBASE_UPDATES, " -> diff %lld, updateDiff %lld, time %lld, updateTime %lld\n", diff, _updateDiff, event.time_stamp, _updateTime);

        if (_updateDiff == 0 || (diff - _updateDiff < -7000) || event.surface_id == _updateID || 2 * diff < (event.time_stamp - _updateTime))
        {
            _updateDiff = diff;
            _updateID = event.surface_id;
            _updateFlipCount = event.flip_count;
            _updateTime = event.time_stamp;
        }
    }

}

void
AppBase::attachDFBWindow(Window* window)
{
    if (window)
    {
        DFBResult ret;

        ret = window->_dfbWindow->AttachEventBuffer(window->_dfbWindow, __buffer);
        if (ret != DFB_OK)
            ILOG_ERROR( ILX_APPBASE, "AttachEventBuffer error: %s!\n", DirectFBErrorString(ret));

        ret = window->_dfbWindow->RequestFocus(window->_dfbWindow);
        if (ret != DFB_OK)
            ILOG_ERROR( ILX_APPBASE, "RequestFocus error: %s! \n", DirectFBErrorString(ret));

        ret = __buffer->Reset(__buffer);
        if (ret != DFB_OK)
            ILOG_ERROR( ILX_APPBASE, "Buffer reset error: %s!\n", DirectFBErrorString(ret));

        ILOG_DEBUG(ILX_APPBASE, "Window %p is attached.\n", window);
    }
}

void
AppBase::detachDFBWindow(Window* window)
{
    if (window)
    {
        DFBResult ret;

        ret = window->_dfbWindow->DetachEventBuffer(window->_dfbWindow, __buffer);
        if (ret != DFB_OK)
            ILOG_ERROR( ILX_APPBASE, "DetachEventBuffer error: %s!\n", DirectFBErrorString(ret));

        ret = __buffer->Reset(__buffer);
        if (ret != DFB_OK)
            ILOG_ERROR(ILX_APPBASE, "Buffer reset error: %s", DirectFBErrorString(ret));

        ILOG_DEBUG(ILX_APPBASE, "Window %p is detached.\n", window);
    }
}

void
AppBase::handleKeyInputEvent(const DFBInputEvent& event, DFBWindowEventType type)
{
    DFBEvent we;
    we.clazz = DFEC_WINDOW;
    we.window.type = type;

    we.window.flags = (event.flags & DIEF_REPEAT) ? DWEF_REPEAT : DWEF_NONE;
    we.window.key_code = event.key_code;
    we.window.key_id = event.key_id;
    we.window.key_symbol = event.key_symbol;

    we.window.locks = event.locks;
    we.window.modifiers = event.modifiers;
    we.window.button = event.button;
    we.window.buttons = event.buttons;

    if (!windowPreEventFilter((const DFBWindowEvent&) we))
        activeWindow()->handleWindowEvent((const DFBWindowEvent&) we);
}

void
AppBase::handleButtonInputEvent(const DFBInputEvent& event, DFBWindowEventType type)
{
    DFBEvent we;
    we.clazz = DFEC_WINDOW;

    we.window.type = type;
    we.window.x = __cursorNew.x;
    we.window.y = __cursorNew.y;

    we.window.cx = __cursorNew.x;
    we.window.cy = __cursorNew.y;

    we.window.button = event.button;
    we.window.buttons = event.buttons;

    if (!windowPreEventFilter((const DFBWindowEvent&) we))
        activeWindow()->handleWindowEvent((const DFBWindowEvent&) we);
}

void
AppBase::handleAxisMotion(const DFBInputEvent& event)
{
    DFBEvent we;
    we.clazz = DFEC_WINDOW;
    we.window.type = DWET_MOTION;

    we.window.step = 0;

    __cursorOld = __cursorNew;

    if (event.flags & DIEF_AXISREL)
    {
        if (event.axis == DIAI_X)
            __cursorNew.x += event.axisrel;
        else if (event.axis == DIAI_Y)
            __cursorNew.y += event.axisrel;
        else
        {
            we.window.type = DWET_WHEEL;
            we.window.step = -event.axisrel;
        }
    } else if (event.flags & DIEF_AXISABS)
    {
        if (event.axis == DIAI_X)
            __cursorNew.x = (event.axisabs - event.min) * __layerSize.w / (event.max - event.min);
        else if (event.axis == DIAI_Y)
            __cursorNew.y = (event.axisabs - event.min) * __layerSize.h / (event.max - event.min);
        else
        {
            we.window.type = DWET_WHEEL;
            we.window.step = -event.axisrel;
        }
    }

    if (__cursorNew.x < 0)
        __cursorNew.x = 0;
    else if (__cursorNew.x > __layerSize.w - 1)
        __cursorNew.x = __layerSize.w - 1;

    if (__cursorNew.y < 0)
        __cursorNew.y = 0;
    else if (__cursorNew.y > __layerSize.h - 1)
        __cursorNew.y = __layerSize.h - 1;

    if (we.window.type == DWET_MOTION && PlatformManager::instance().cursorVisible())
    {
        Rectangle cold(__instance->__cursorOld.x, __instance->__cursorOld.y, 32, 32);
        Rectangle cnew(__instance->__cursorNew.x, __instance->__cursorNew.y, 32, 32);

        activeWindow()->update(PaintEvent(cnew.united(cold), 10));
    }

    we.window.x = 0;
    we.window.y = 0;

    we.window.x = __cursorNew.x;
    we.window.y = __cursorNew.y;

    we.window.cx = __cursorNew.x;
    we.window.cy = __cursorNew.y;

    we.window.button = event.button;
    we.window.buttons = event.buttons;

    if (!windowPreEventFilter((const DFBWindowEvent&) we))
        activeWindow()->handleWindowEvent((const DFBWindowEvent&) we);
}

void
AppBase::disableSurfaceEventSync(long long micros)
{
    ILOG_TRACE_F(ILX_APPBASE_UPDATES);
    _updateDisable = direct_clock_get_time(DIRECT_CLOCK_MONOTONIC) + micros;
    ILOG_DEBUG(ILX_APPBASE_UPDATES, " ->   disable %16lld\n", _updateDisable);
}

} /* namespace ilixi */
