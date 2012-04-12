/*
 Copyright 2011 Tarik Sekmen.

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

#include "core/Thread.h"
#include "core/Logger.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

using namespace ilixi;
// TODO Move thread to lib
D_DEBUG_DOMAIN( ILX_THREAD, "ilixi/lib/Thread", "Thread");

Thread::Thread() :
    _running(false), _code(0), _stackSize(0)
{
  pthread_mutex_init(&_runMutex, NULL);
}

Thread::~Thread()
{
  cancel();
  pthread_mutex_destroy(&_runMutex);
}

bool
Thread::start()
{
  pthread_mutex_lock(&_runMutex);
  if (_running)
    return false;
  pthread_mutex_unlock(&_runMutex);

  int rc = 0;
  pthread_attr_t attr;

  rc = pthread_attr_init(&attr);
  if (rc != 0)
    {
      ILOG_ERROR(ILX_THREAD, "pthread_attr_init: %d", rc);
      return false;
    }

  rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  if (rc != 0)
    {
      ILOG_ERROR(ILX_THREAD, "pthread_attr_setdetachstate: %d", rc);
      return false;
    }

  if (_stackSize > 0)
    {
      void *sp;

      rc = posix_memalign(&sp, sysconf(_SC_PAGESIZE), _stackSize);
      if (rc != 0)
        {
          ILOG_ERROR(ILX_THREAD, "posix_memalign: %d", rc);
          return false;
        }
      ILOG_DEBUG(ILX_THREAD, "posix_memalign() allocated at %p", sp);

      rc = pthread_attr_setstack(&attr, sp, _stackSize);
      if (rc != 0)
        {
          ILOG_ERROR(ILX_THREAD, "pthread_attr_setstack: %d", rc);
          return false;
        }
    }

  rc = pthread_create(&_pThread, &attr, Thread::wrapper, (void*) this);
  if (rc != 0)
    {
      ILOG_ERROR(ILX_THREAD, "Unable to create thread: %d", rc);
      return false;
    }

  rc = pthread_attr_destroy(&attr);
  if (rc != 0)
    {
      ILOG_ERROR(ILX_THREAD, "pthread_attr_destroy: %d", rc);
      return false;
    }

  return true;
}

bool
Thread::cancel()
{
  pthread_mutex_lock(&_runMutex);
  if (!_running)
    return false;
  _running = false;
  pthread_mutex_unlock(&_runMutex);

  void *status = NULL;
  int rc = 0;
  rc = pthread_cancel(_pThread);
  if (rc)
    ILOG_ERROR(ILX_THREAD, "pthread_cancel error: %d", rc);

  pthread_join(_pThread, &status);
  if (status != PTHREAD_CANCELED)
    {
      ILOG_ERROR(ILX_THREAD,
          "Thread (id: %lu) returned unexpected result!", _pThread);
      return false;
    }

  sigTerminated();
  ILOG_INFO(ILX_THREAD, "Thread (id: %lu) cancelled.", _pThread);
  return true;
}

bool
Thread::join()
{
  pthread_mutex_lock(&_runMutex);
  if (!_running)
    return false;
  _running = false;
  pthread_mutex_unlock(&_runMutex);

  void *status = NULL;
  int rc = 0;
  rc = pthread_join(_pThread, &status);

  switch (rc)
    {
  case EDEADLK:
    ILOG_ERROR(ILX_THREAD,
        "Thread (id: %lu) A deadlock was detected.", _pThread);
    return false;
  case EINVAL:
    ILOG_ERROR(ILX_THREAD, "Thread (id: %lu) is not joinable.", _pThread);
    return false;
  case ESRCH:
    ILOG_ERROR(ILX_THREAD,
        "No thread with the id (%lu) could be found.", _pThread);
    return false;
  case 0:
    ILOG_DEBUG(ILX_THREAD, "Thread (id: %lu) joined.", _pThread);
    return true;
  default:
    ILOG_ERROR(ILX_THREAD, "pthread_join error: %d", rc);
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
  sigStarted();
  pthread_mutex_lock(&_runMutex);
  _running = true;
  pthread_mutex_unlock(&_runMutex);

  ILOG_DEBUG(ILX_THREAD, "Thread (id: %lu) is running.", _pThread);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

  _code = run();

  pthread_mutex_lock(&_runMutex);
  _running = false;
  pthread_mutex_unlock(&_runMutex);
  ILOG_DEBUG(ILX_THREAD,
      "Thread (id: %lu) completed its execution with code: %d.", _pThread, _code);
  sigFinished();
}

void*
Thread::wrapper(void* arg)
{
  ((Thread*) arg)->executeRun();
  return NULL;
}
