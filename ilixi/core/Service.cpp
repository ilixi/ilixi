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

#include <core/Service.h>

#include <core/Logger.h>
#include <core/PlatformManager.h>
#include <vector>
#include <algorithm>

namespace ilixi
{

IDirectFBEventBuffer* Service::__buffer = NULL;
Service* Service::__instance = NULL;

D_DEBUG_DOMAIN( ILX_SERVICE, "ilixi/core/Service", "Service");
D_DEBUG_DOMAIN( ILX_SERVICE_LOOP, "ilixi/core/Service/Loop", "Service main loop");

Service::Service(int* argc, char*** argv, AppOptions opts)
        : _terminate(false)
{
    ILOG_TRACE_F(ILX_SERVICE);

    if (__instance)
        ILOG_THROW(ILX_SERVICE, "Cannot allow more than one instance!\n");

    __instance = this;

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&__cbMutex, &attr);
    pthread_mutex_init(&__timerMutex, &attr);
    pthread_mutexattr_destroy(&attr);

    PlatformManager::instance().initialize(argc, argv, opts);
    initEventBuffer();
}

Service::~Service()
{
    ILOG_TRACE_F(ILX_SERVICE);
    releaseEventBuffer();
    PlatformManager::instance().release();

    pthread_mutex_destroy(&__cbMutex);
    pthread_mutex_destroy(&__timerMutex);

    __instance = NULL;
}

void
Service::exec()
{
    ILOG_INFO(ILX_SERVICE, "Starting...\n");

    while (true)
    {
        if (__instance->_terminate)
            break;
        else
        {
            __instance->runCallbacks();
            __instance->handleEvents(__instance->runTimers());
            __instance->sigPerformWork();
        }
    }

    ILOG_INFO(ILX_SERVICE, "Stopping...\n");
}

void
Service::wakeUp()
{
    if (__instance)
        __instance->__buffer->WakeUp(__buffer);
}

void
Service::quit()
{
    _terminate = true;
}

void
Service::postUserEvent(unsigned int type, void* data)
{
    DFBUserEvent event;
    event.clazz = DFEC_USER;
    event.type = type;
    event.data = data;
    DFBResult ret = __buffer->PostEvent(__buffer, DFB_EVENT(&event) );
    if (ret != DFB_OK)
        ILOG_ERROR(ILX_SERVICE, "Cannot post UserEvent!\n");
}

void
Service::handleUserEvent(const DFBUserEvent& event)
{
}

static bool
timerSort(const Timer* first, const Timer* second)
{
    return first->expiry() < second->expiry();
}

int32_t
Service::runTimers()
{
    ILOG_TRACE_F(ILX_SERVICE_LOOP);
    int32_t timeout = 10000;

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

    ILOG_DEBUG(ILX_SERVICE_LOOP, " -> desired timeout %d.%d seconds\n", (int) (timeout / 1000), (int)(timeout % 1000));
    return timeout;
}

void
Service::runCallbacks()
{
    ILOG_TRACE_F(ILX_SERVICE_LOOP);

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
            ILOG_DEBUG(ILX_SERVICE_LOOP, " -> Callback %p is removed.\n", ((Callback*) *it2));
            removeCallback(*it2);
        }
        ++it2;
    }
    pthread_mutex_unlock(&__cbMutex);
}

void
Service::handleEvents(int32_t timeout, bool forceWait)
{
    ILOG_TRACE_F(ILX_SERVICE_LOOP);
    DFBEvent event;
    DFBWindowEvent lastMotion; // Used for compressing motion events.

    lastMotion.type = DWET_NONE;

    if (timeout < 1)
        ILOG_ERROR(ILX_SERVICE, "Timeout error with value %d\n", timeout);

    bool wait = true;
    if (timeout)
    {
        if (__callbacks.size())
            wait = false;
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

        ILOG_DEBUG(ILX_SERVICE_LOOP, " -> wait for event with timeout: %d.%d seconds\n", timeout / 1000, timeout % 1000);
        __buffer->WaitForEventWithTimeout(__buffer, timeout / 1000, timeout % 1000);
        ILOG_DEBUG(ILX_SERVICE_LOOP, " -> got event! \n");
    }

    while (__buffer->GetEvent(__buffer, &event) == DFB_OK)
    {
        switch (event.clazz)
        {
        case DFEC_USER:
            handleUserEvent((const DFBUserEvent&) event);
            break;

        default:
            break;
        }
    }

    ILOG_DEBUG(ILX_SERVICE_LOOP, " -> end handle events \n");
}

void
Service::initEventBuffer()
{
    ILOG_TRACE_F(ILX_SERVICE);
    if (PlatformManager::instance().appOptions() & OptExclusive)
    {
        if (PlatformManager::instance().getDFB()->CreateInputEventBuffer(PlatformManager::instance().getDFB(), DICAPS_ALL, DFB_TRUE, &__buffer) != DFB_OK)
            ILOG_THROW(ILX_SERVICE, "Error while creating input event buffer!\n");
    } else if (PlatformManager::instance().getDFB()->CreateEventBuffer(PlatformManager::instance().getDFB(), &__buffer) != DFB_OK)
        ILOG_THROW(ILX_SERVICE, "Error while creating event buffer!\n");
}

void
Service::releaseEventBuffer()
{
    if (__buffer)
    {
        ILOG_DEBUG(ILX_SERVICE, "Releasing event buffer.\n");
        __buffer->Release(__buffer);
        __buffer = NULL;
    }
}

bool
Service::addCallback(Callback* cb)
{
    if (__instance && cb)
    {
        pthread_mutex_lock(&__instance->__cbMutex);

        CallbackList::iterator it = std::find(__instance->__callbacks.begin(), __instance->__callbacks.end(), cb);
        if (cb == *it)
        {
            pthread_mutex_unlock(&__instance->__cbMutex);
            ILOG_DEBUG(ILX_SERVICE, "Callback %p already added!\n", cb);
            return false;
        }
        __instance->__callbacks.push_back(cb);

        pthread_mutex_unlock(&__instance->__cbMutex);
        ILOG_DEBUG(ILX_SERVICE, "Callback %p is added.\n", cb);
        return true;
    }
    return false;
}

bool
Service::removeCallback(Callback* cb)
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
                ILOG_DEBUG(ILX_SERVICE, "Callback %p is removed.\n", cb);
                return true;
            }
        }

        pthread_mutex_unlock(&__instance->__cbMutex);
    }
    return false;
}

bool
Service::addTimer(Timer* timer)
{
    if (__instance && timer)
    {
        pthread_mutex_lock(&__instance->__timerMutex);
        TimerList::iterator it = std::find(__instance->_timers.begin(), __instance->_timers.end(), timer);
        if (timer == *it)
        {
            pthread_mutex_unlock(&__instance->__timerMutex);
            ILOG_DEBUG(ILX_SERVICE, "Timer %p already added!\n", timer);
            return false;
        }
        __instance->_timers.push_back(timer);
        __instance->_timers.sort(timerSort);
        pthread_mutex_unlock(&__instance->__timerMutex);
        __instance->__buffer->WakeUp(__buffer);
        ILOG_DEBUG(ILX_SERVICE, "Timer %p is added.\n", timer);
        return true;
    }
    return false;
}

bool
Service::removeTimer(Timer* timer)
{
    if (__instance && timer)
    {
        pthread_mutex_lock(&__instance->__timerMutex);
        for (TimerList::iterator it = __instance->_timers.begin(); it != __instance->_timers.end(); ++it)
        {
            if (timer == *it)
            {
                __instance->_timers.erase(it);
                __instance->_timers.sort(timerSort);
                pthread_mutex_unlock(&__instance->__timerMutex);
                ILOG_DEBUG(ILX_SERVICE, "Timer %p is removed.\n", timer);
                return true;
            }
        }
        pthread_mutex_unlock(&__instance->__timerMutex);
        ILOG_DEBUG(ILX_SERVICE, "Could not remove Timer %p, not found!\n", timer);
    }
    return false;
}

} /* namespace ilixi */
