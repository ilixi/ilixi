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

#include <lib/Thread.h>
#include <core/Logger.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#ifdef ANDROID_NDK
extern "C" {
#include <compat/pthread_cancel.h>
}
#endif

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_THREAD, "ilixi/lib/Thread", "Thread");

Thread::Thread()
        : _running(false),
          _code(0),
          _stackSize(0)
{
    ILOG_TRACE_F(ILX_THREAD);
    pthread_mutex_init(&_runMutex, NULL);
}

Thread::~Thread()
{
    ILOG_TRACE_F(ILX_THREAD);
    cancel();
    pthread_mutex_destroy(&_runMutex);
}

bool
Thread::start()
{
    ILOG_TRACE_F(ILX_THREAD);
    if (_running)
        return false;

    int rc = 0;
    pthread_attr_t attr;

    rc = pthread_attr_init(&attr);
    if (rc != 0)
    {
        ILOG_ERROR(ILX_THREAD, "pthread_attr_init: %d\n", rc);
        return false;
    }

    rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if (rc != 0)
    {
        ILOG_ERROR(ILX_THREAD, "pthread_attr_setdetachstate: %d\n", rc);
        return false;
    }

#ifndef ANDROID_NDK
    if (_stackSize > 0)
    {
        void *sp;

        rc = posix_memalign(&sp, sysconf(_SC_PAGESIZE), _stackSize);
        if (rc != 0)
        {
            ILOG_ERROR(ILX_THREAD, "posix_memalign: %d\n", rc);
            return false;
        }
        ILOG_DEBUG(ILX_THREAD, "posix_memalign() allocated at %p\n", sp);

        rc = pthread_attr_setstack(&attr, sp, _stackSize);
        if (rc != 0)
        {
            ILOG_ERROR(ILX_THREAD, "pthread_attr_setstack: %d\n", rc);
            return false;
        }
    }
#endif

    rc = pthread_create(&_pThread, &attr, Thread::wrapper, (void*) this);
    if (rc != 0)
    {
        ILOG_ERROR(ILX_THREAD, "Unable to create thread: %d\n", rc);
        return false;
    }

    rc = pthread_attr_destroy(&attr);
    if (rc != 0)
    {
        ILOG_ERROR(ILX_THREAD, "pthread_attr_destroy: %d\n", rc);
        return false;
    }

    return true;
}

bool
Thread::cancel()
{
    ILOG_TRACE_F(ILX_THREAD);

    if (!_running)
        return false;

    void *status = NULL;
    int rc = 0;
    rc = pthread_cancel(_pThread);
    if (rc)
        ILOG_ERROR(ILX_THREAD, "pthread_cancel error: %d\n", rc);

    pthread_join(_pThread, &status);

    pthread_mutex_lock(&_runMutex);
    _running = false;
    pthread_mutex_unlock(&_runMutex);

#ifndef ANDROID_NDK
    if (status != PTHREAD_CANCELED )
    {
        ILOG_ERROR(ILX_THREAD, "Thread (id: %lu) returned unexpected result!\n", _pThread);
        return false;
    }
#endif

    sigTerminated();
    ILOG_DEBUG(ILX_THREAD, "Thread (id: %lu) cancelled.\n", _pThread);
    return true;
}

bool
Thread::join()
{
    ILOG_TRACE_F(ILX_THREAD);

    void *status = NULL;
    int rc = 0;
    rc = pthread_join(_pThread, &status);

    pthread_mutex_lock(&_runMutex);
    if (!_running)
        return false;
    _running = false;
    pthread_mutex_unlock(&_runMutex);

    switch (rc)
    {
    case EDEADLK:
        ILOG_ERROR(ILX_THREAD, "Thread (id: %lu) A deadlock was detected.\n", _pThread);
        return false;
    case EINVAL:
        ILOG_ERROR(ILX_THREAD, "Thread (id: %lu) is not joinable.\n", _pThread);
        return false;
    case ESRCH:
        ILOG_ERROR(ILX_THREAD, "No thread with the id (%lu) could be found.\n", _pThread);
        return false;
    case 0:
        ILOG_DEBUG(ILX_THREAD, "Thread (id: %lu) joined.\n", _pThread);
        return true;
    default:
        ILOG_ERROR(ILX_THREAD, "pthread_join error: %d\n", rc);
        return false;
    }
}

int
Thread::exit_code()
{
    return _code;
}

int
Thread::stackSize() const
{
    return _stackSize;
}

void
Thread::setStackSize(size_t stackSize)
{
    _stackSize = stackSize;
}

void
Thread::executeRun()
{
    ILOG_TRACE_F(ILX_THREAD);
    sigStarted();
    pthread_mutex_lock(&_runMutex);
    _running = true;
    pthread_mutex_unlock(&_runMutex);

    ILOG_DEBUG(ILX_THREAD, "Thread (id: %lu) is running.\n", _pThread);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    _code = run();

    pthread_mutex_lock(&_runMutex);
    _running = false;
    pthread_mutex_unlock(&_runMutex);
    ILOG_DEBUG( ILX_THREAD, "Thread (id: %lu) completed its execution with code: %d.\n", _pThread, _code);
    sigFinished();
}

void*
Thread::wrapper(void* arg)
{
    ((Thread*) arg)->executeRun();
    return NULL;
}

} /* namespace ilixi */
