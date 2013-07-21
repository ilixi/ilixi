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

#include <core/Engine.h>

#include <core/Logger.h>
#include <core/PlatformManager.h>

#include <algorithm>

#include <directfb_util.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_ENGINE, "ilixi/core/Engine", "Engine");
D_DEBUG_DOMAIN(ILX_ENGINE_LOOP, "ilixi/core/Engine_Cycle", "Engine cycle");
D_DEBUG_DOMAIN(ILX_ENGINE_UPDATES, "ilixi/core/Engine/Updates", "Engine Updates");

static bool
timerSort(const Timer* first, const Timer* second)
{
    return first->expiry() < second->expiry();
}

Engine&
Engine::instance()
{
    static Engine instance;
    return instance;
}

Engine::Engine()
        : __buffer(NULL),
          _terminate(false)
{
    ILOG_TRACE(ILX_ENGINE);
}

Engine::~Engine()
{
    ILOG_TRACE(ILX_ENGINE);
}

void
Engine::initialise()
{
    ILOG_TRACE(ILX_ENGINE);
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&__cbMutex, &attr);
    pthread_mutex_init(&__timerMutex, &attr);
#if ILIXI_HAS_SURFACEEVENTS
    pthread_mutex_init(&__selMutex, NULL);
#endif
    pthread_mutexattr_destroy(&attr);

    initEventBuffer();

}

void
Engine::release()
{
    ILOG_TRACE(ILX_ENGINE);
    stop();
    releaseEventBuffer();
    pthread_mutex_destroy(&__cbMutex);
    pthread_mutex_destroy(&__timerMutex);
#if ILIXI_HAS_SURFACEEVENTS
    pthread_mutex_destroy(&__selMutex);
#endif
}

int32_t
Engine::cycle()
{
    runCallbacks();
    sigPerformWork();
    return runTimers();
}

void
Engine::wakeUp()
{
    ILOG_TRACE(ILX_ENGINE);
    __buffer->WakeUp(__buffer);
}

bool
Engine::stopped() const
{
    return _terminate;
}

void
Engine::stop()
{
    ILOG_TRACE(ILX_ENGINE);
    _terminate = true;
    wakeUp();
}

bool
Engine::addCallback(Callback* cb)
{
    ILOG_TRACE(ILX_ENGINE);
    if (cb)
    {
        pthread_mutex_lock(&__cbMutex);

        CallbackList::iterator it = std::find(__callbacks.begin(), __callbacks.end(), cb);
        if (cb == *it)
        {
            pthread_mutex_unlock(&__cbMutex);
            ILOG_DEBUG(ILX_ENGINE, "Callback %p already added!\n", cb);
            return false;
        }
        __callbacks.push_back(cb);

        pthread_mutex_unlock(&__cbMutex);
        ILOG_DEBUG(ILX_ENGINE, "Callback %p is added.\n", cb);
        return true;
    }
    return false;
}

bool
Engine::removeCallback(Callback* cb)
{
    ILOG_TRACE(ILX_ENGINE);
    if (cb)
    {
        pthread_mutex_lock(&__cbMutex);

        for (CallbackList::iterator it = __callbacks.begin(); it != __callbacks.end(); ++it)
        {
            if (cb == *it)
            {
                __callbacks.erase(it);
                pthread_mutex_unlock(&__cbMutex);
                ILOG_DEBUG(ILX_ENGINE, "Callback %p is removed.\n", cb);
                return true;
            }
        }

        pthread_mutex_unlock(&__cbMutex);
    }
    return false;
}

unsigned int
Engine::numCallbacks() const
{
    return __callbacks.size();
}

bool
Engine::addTimer(Timer* timer)
{
    ILOG_TRACE(ILX_ENGINE);
    if (timer)
    {
        pthread_mutex_lock(&__timerMutex);
        TimerList::iterator it = std::find(_timers.begin(), _timers.end(), timer);
        if (timer == *it)
        {
            pthread_mutex_unlock(&__timerMutex);
            ILOG_DEBUG(ILX_ENGINE, "Timer %p already added!\n", timer);
            return false;
        }
        _timers.push_back(timer);
        _timers.sort(timerSort);
        pthread_mutex_unlock(&__timerMutex);
        __buffer->WakeUp(__buffer);
        ILOG_DEBUG(ILX_ENGINE, "Timer %p is added.\n", timer);
        return true;
    }
    return false;
}

bool
Engine::removeTimer(Timer* timer)
{
    ILOG_TRACE(ILX_ENGINE);
    if (timer)
    {
        pthread_mutex_lock(&__timerMutex);
        for (TimerList::iterator it = _timers.begin(); it != _timers.end(); ++it)
        {
            if (timer == *it)
            {
                _timers.erase(it);
                _timers.sort(timerSort);
                pthread_mutex_unlock(&__timerMutex);
                ILOG_DEBUG(ILX_ENGINE, "Timer %p is removed.\n", timer);
                return true;
            }
        }
        pthread_mutex_unlock(&__timerMutex);
        ILOG_DEBUG(ILX_ENGINE, "Could not remove Timer %p, not found!\n", timer);
    }
    return false;
}

void
Engine::postUniversalEvent(Widget* target, unsigned int type, void* data)
{
    ILOG_TRACE(ILX_ENGINE);
    UniversalEvent event(target, type, data);
    DFBResult ret = __buffer->PostEvent(__buffer, DFB_EVENT(&event) );
    if (ret != DFB_OK)
        ILOG_ERROR(ILX_ENGINE, "Cannot post UniversalEvent!\n");
}

void
Engine::postUserEvent(unsigned int type, void* data)
{
    ILOG_TRACE(ILX_ENGINE);
    DFBUserEvent event;
    event.clazz = DFEC_USER;
    event.type = type;
    event.data = data;
    DFBResult ret = __buffer->PostEvent(__buffer, DFB_EVENT(&event) );
    if (ret != DFB_OK)
        ILOG_ERROR(ILX_ENGINE, "Cannot post UserEvent!\n");
}

void
Engine::postEvent(DFBEvent* event)
{
    ILOG_TRACE(ILX_ENGINE);
    DFBResult ret = __buffer->PostEvent(__buffer, event);
    if (ret != DFB_OK)
        ILOG_ERROR(ILX_ENGINE, "Cannot post DFBEvent!\n");
}

void
Engine::runCallbacks()
{
    ILOG_TRACE(ILX_ENGINE_LOOP);

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
            ILOG_DEBUG(ILX_ENGINE_LOOP, " -> Callback %p is removed.\n", ((Callback*) *it2));
            removeCallback(*it2);
        }
        ++it2;
    }
    pthread_mutex_unlock(&__cbMutex);
}

int32_t
Engine::runTimers()
{
    ILOG_TRACE(ILX_ENGINE_LOOP);
    int32_t timeout = 100;

    pthread_mutex_lock(&__timerMutex);
    if (_timers.size())
    {
        int64_t now = direct_clock_get_millis();
        int64_t expiry = _timers.front()->expiry();

        if (expiry <= now)
        {
            if (!_timers.front()->funck())
                _timers.pop_front();

            _timers.sort(timerSort);

            if (_timers.size())
                timeout = _timers.front()->expiry() - now;
        } else
            timeout = expiry - now;
    }
    pthread_mutex_unlock(&__timerMutex);
    if (timeout < 1)
        timeout = 1;
    else if (timeout > 10000)
        timeout = 10000;

    ILOG_DEBUG(ILX_ENGINE_LOOP, " -> desired timeout %d.%d seconds\n", (int) (timeout / 1000), (int)(timeout % 1000));
    return timeout;
}

void
Engine::initEventBuffer()
{
    ILOG_TRACE(ILX_ENGINE);
    if (PlatformManager::instance().appOptions() & OptExclusive)
    {
        if (PlatformManager::instance().getDFB()->CreateInputEventBuffer(PlatformManager::instance().getDFB(), DICAPS_ALL, DFB_TRUE, &__buffer) != DFB_OK)
            ILOG_THROW(ILX_ENGINE, "Error while creating input event buffer!\n");
    } else if (PlatformManager::instance().getDFB()->CreateEventBuffer(PlatformManager::instance().getDFB(), &__buffer) != DFB_OK)
        ILOG_THROW(ILX_ENGINE, "Error while creating event buffer!\n");
}

void
Engine::releaseEventBuffer()
{
    if (__buffer)
    {
        ILOG_DEBUG(ILX_ENGINE, "Releasing event buffer.\n");
        __buffer->Release(__buffer);
        __buffer = NULL;
    }
}

DFBResult
Engine::resetBuffer()
{
    ILOG_TRACE(ILX_ENGINE);
    DFBResult ret = __buffer->Reset(__buffer);
    if (ret != DFB_OK)
        ILOG_ERROR(ILX_ENGINE, "Buffer reset error: %s!\n", DirectFBErrorString(ret));
    return ret;
}

DFBResult
Engine::attachWindow(IDirectFBWindow* dfbWindow)
{
    ILOG_TRACE(ILX_ENGINE);
    DFBResult ret = dfbWindow->AttachEventBuffer(dfbWindow, __buffer);
    if (ret != DFB_OK)
        ILOG_ERROR(ILX_ENGINE, "AttachEventBuffer error: %s!\n", DirectFBErrorString(ret));
    return ret;
}

DFBResult
Engine::detachWindow(IDirectFBWindow* dfbWindow)
{
    ILOG_TRACE(ILX_ENGINE);
    DFBResult ret = dfbWindow->DetachEventBuffer(dfbWindow, __buffer);
    if (ret != DFB_OK)
        ILOG_ERROR(ILX_ENGINE, "DetachEventBuffer error: %s!\n", DirectFBErrorString(ret));
    return ret;
}

DFBResult
Engine::getNextEvent(DFBEvent* event)
{
    return __buffer->GetEvent(__buffer, event);
}

void
Engine::waitForEvents(int32_t timeout)
{
    ILOG_TRACE(ILX_ENGINE);
    DFBEvent event;

    if (timeout < 1)
        ILOG_ERROR(ILX_ENGINE_LOOP, "Timeout error with value %d\n", timeout);
    else if (timeout && Engine::instance().numCallbacks())
    {
        // do not wait
    } else
    {
        // discard window update event in buffer.
        DFBResult ret = __buffer->PeekEvent(__buffer, &event);
        if (ret == DFB_OK)
        {
            if ((event.clazz == DFEC_WINDOW) && (event.window.type == DWET_UPDATE))
                __buffer->GetEvent(__buffer, &event);
        }

        ILOG_DEBUG(ILX_ENGINE_LOOP, " -> wait for event with timeout: %d.%d seconds\n", timeout / 1000, timeout % 1000);
        __buffer->WaitForEventWithTimeout(__buffer, timeout / 1000, timeout % 1000);
        ILOG_DEBUG(ILX_ENGINE_LOOP, " -> end wait for event! \n");
    }
}

#if ILIXI_HAS_SURFACEEVENTS
bool
Engine::addSurfaceEventListener(SurfaceEventListener* sel)
{
    ILOG_TRACE(ILX_ENGINE);
    if (sel)
    {
        pthread_mutex_lock(&__selMutex);

        SurfaceListenerList::iterator it = std::find(__selList.begin(), __selList.end(), sel);
        if (sel == *it)
        {
            pthread_mutex_unlock(&__selMutex);
            ILOG_ERROR(ILX_ENGINE, "SurfaceEventListener %p already added!\n", sel);
            return false;
        }

        ILOG_DEBUG(ILX_ENGINE, "SurfaceEventListener %p is added.\n", sel);
        // check whether to attach source surface
        bool attach = true;
        for (SurfaceListenerList::iterator it = __selList.begin(); it != __selList.end(); ++it)
        {
            if (sel->sourceSurface() == ((SurfaceEventListener*) *it)->sourceSurface())
                attach = false;
        }

        if (attach)
        {
            sel->sourceSurface()->MakeClient(sel->sourceSurface());
            sel->sourceSurface()->AttachEventBuffer(sel->sourceSurface(), __buffer);
            ILOG_DEBUG(ILX_ENGINE, " -> Surface[%p] is attached.\n", sel->sourceSurface());
        }

        __selList.push_back(sel);
        pthread_mutex_unlock(&__selMutex);
        return true;
    }
    return false;
}

bool
Engine::removeSurfaceEventListener(SurfaceEventListener* sel)
{
    ILOG_TRACE(ILX_ENGINE);
    if (sel)
    {
        pthread_mutex_lock(&__selMutex);

        bool ret = false;
        IDirectFBSurface* source = sel->sourceSurface();

        for (SurfaceListenerList::iterator it = __selList.begin(); it != __selList.end(); ++it)
        {
            if (sel == *it)
            {
                __selList.erase(it);
                ILOG_DEBUG(ILX_ENGINE, "SurfaceEventListener %p is removed.\n", sel);
                ret = true;
                break;
            }
        }

        if (ret)
        {
            bool detach = true;
            for (SurfaceListenerList::iterator it = __selList.begin(); it != __selList.end(); ++it)
            {
                if (source == ((SurfaceEventListener*) *it)->sourceSurface())
                    detach = false;
            }
            pthread_mutex_unlock(&__selMutex);

            if (detach)
            {
                sel->sourceSurface()->DetachEventBuffer(sel->sourceSurface(), __buffer);
                ILOG_DEBUG(ILX_ENGINE, " -> Surface[%p] is detached.\n", sel->sourceSurface());
            }
            return true;
        } else
        {
            pthread_mutex_unlock(&__selMutex);
            return false;
        }
    }
    return false;
}

void
Engine::consumeSurfaceEvent(const DFBSurfaceEvent& event)
{
    pthread_mutex_lock(&__selMutex);

    ILOG_DEBUG(ILX_ENGINE_UPDATES, " -> SURFACE EVENT [%3d]  %4d,%4d-%4dx%4d (count %d)\n", event.surface_id, DFB_RECTANGLE_VALS_FROM_REGION(&event.update), event.flip_count);
    for (SurfaceListenerList::iterator it = __selList.begin(); it != __selList.end(); ++it)
        ((SurfaceEventListener*) *it)->consumeSurfaceEvent(event);

    pthread_mutex_unlock(&__selMutex);
}

#endif // end ILIXI_HAS_SURFACEEVENTS
} /* namespace ilixi */
