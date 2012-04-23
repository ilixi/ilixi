/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#include "core/AppBase.h"
#include "ui/WindowWidget.h"
#include "core/Logger.h"
#include <algorithm>

using namespace std;
using namespace ilixi;
using namespace IMaestro;

IDirectFB* AppBase::__dfb = NULL;
IDirectFBDisplayLayer* AppBase::__layer = NULL;
IDirectFBEventBuffer* AppBase::__buffer = NULL;
AppBase* AppBase::__instance = NULL;

D_DEBUG_DOMAIN( ILX_APPBASE, "ilixi/core/AppBase", "AppBase");

AppBase::AppBase() :
    __title(""), __state(Hidden), __activeWindow(NULL)
{
  if (__instance)
    exit(1);
  __instance = this;
  pthread_mutex_init(&__cbMutex, NULL);
  pthread_mutex_init(&__selMutex, NULL);
  pthread_mutex_init(&__windowMutex, NULL);
}

AppBase::AppBase(int argc, char* argv[]) :
    __title(""), __state(Hidden), __activeWindow(NULL)
{
  if (__instance)
    exit(1);
  __instance = this;
  pthread_mutex_init(&__cbMutex, NULL);
  pthread_mutex_init(&__selMutex, NULL);
  pthread_mutex_init(&__windowMutex, NULL);
}

AppBase::~AppBase()
{
  releaseDFB();
  ILOG_DEBUG(ILX_APPBASE, "~AppBase\n");
}

std::string
AppBase::title() const
{
  return (__title);
}

void
AppBase::setTitle(std::string title)
{
  __title = title;
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

bool
AppBase::windowPostEventFilter(const DFBWindowEvent& event)
{
  return false;
}

AppState
AppBase::appState() const
{
  return (__state);
}

void
AppBase::setAppState(AppState state)
{
  __state = (AppState) (__state | state);
}

void
AppBase::clearAppState(AppState state)
{
  __state = (AppState) (__state & ~state);
}

IDirectFB*
AppBase::getDFB()
{
  return __dfb;
}

bool
AppBase::initDFB(int argc, char **argv, AppOptions opts)
{
  if (!__dfb)
    {
      ILOG_DEBUG(ILX_APPBASE, "Initialising DirectFB interfaces...\n");

      if (DirectFBInit(&argc, &argv) != DFB_OK)
        {
          ILOG_ERROR(ILX_APPBASE, "Error while initialising DirectFB!\n");
          return false;
        }

      if (DirectFBCreate(&__dfb) != DFB_OK)
        {
          ILOG_ERROR(ILX_APPBASE, "Error while creating DirectFB interface!\n");
          return false;
        }

      if (__dfb->CreateEventBuffer(__dfb, &__buffer) != DFB_OK)
        {
          ILOG_ERROR(ILX_APPBASE, "Error while creating event buffer!\n");
          return false;
        }

      if (__dfb->GetDisplayLayer(__dfb, DLID_PRIMARY, &__layer) != DFB_OK)
        {
          ILOG_ERROR(ILX_APPBASE, "Error while getting primary layer!\n");
          return false;
        }

      if (opts & OptCompositor)
        {
          if (__layer->SetCooperativeLevel(__layer, DLSCL_ADMINISTRATIVE))
            {
              ILOG_ERROR(ILX_APPBASE, "Error while setting EXLUSIVE mode!\n");
              return false;
            }
        }

      ILOG_INFO(ILX_APPBASE, "DirectFB interfaces are ready.\n");

      return true;
    }
  return false;
}

void
AppBase::releaseDFB()
{
  if (__dfb)
    {
      ILOG_DEBUG(ILX_APPBASE, "Releasing DirectFB interfaces...\n");
      __buffer->Release(__buffer);
      __layer->Release(__layer);
      __dfb->Release(__dfb);
      __dfb = NULL;
      __activeWindow = NULL;
      pthread_mutex_destroy(&__cbMutex);
      pthread_mutex_destroy(&__selMutex);
      pthread_mutex_destroy(&__windowMutex);
      ILOG_INFO(ILX_APPBASE, "DirectFB interface is released.\n");
    }
}

IDirectFBDisplayLayer*
AppBase::getLayer()
{
  return __layer;
}

IDirectFBWindow*
AppBase::activeDFBWindow() const
{
  return __activeWindow->_window->_dfbWindow;
}

bool
AppBase::addCallback(Callback* cb)
{
  if (__instance && cb)
    {
      pthread_mutex_lock(&__instance->__cbMutex);

      CallbackList::iterator it = std::find(__instance->__callbacks.begin(),
          __instance->__callbacks.end(), cb);
      if (cb == *it)
        {
          pthread_mutex_unlock(&__instance->__cbMutex);
          ILOG_ERROR(ILX_APPBASE, "Callback %p already added!\n", cb);
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

      for (CallbackList::iterator it = __instance->__callbacks.begin();
          it != __instance->__callbacks.end(); ++it)
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
      ILOG_ERROR(ILX_APPBASE, "Cannot remove callback, %p not found!\n", cb);
    }
  return false;
}

void
AppBase::runCallbacks()
{
  pthread_mutex_lock(&__cbMutex);

  CallbackList::iterator it = __callbacks.begin();
  while (it != __callbacks.end())
    {
      if (!((Callback*) *it)->_func(((Callback*) *it)->_data))
        {
          ILOG_DEBUG(
              ILX_APPBASE, "Callback %p is removed.\n", ((Callback*) *it));
          it = __callbacks.erase(it);
        }
      ++it;
    }

  if (!__callbacks.empty())
    __buffer->WakeUp(__buffer);

  pthread_mutex_unlock(&__cbMutex);
}

bool
AppBase::addSurfaceEventListener(SurfaceEventListener* sel)
{
  if (__instance && sel)
    {
      pthread_mutex_lock(&__instance->__selMutex);

      SurfaceListenerList::iterator it = std::find(
          __instance->__selList.begin(), __instance->__selList.end(), sel);
      if (sel == *it)
        {
          pthread_mutex_unlock(&__instance->__selMutex);
          ILOG_ERROR(
              ILX_APPBASE, "SurfaceEventListener %p already added!\n", sel);
          return false;
        }
      __instance->__selList.push_back(sel);
      pthread_mutex_unlock(&__instance->__selMutex);
      ILOG_DEBUG(ILX_APPBASE, "SurfaceEventListener %p is added.\n", sel);
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

      for (SurfaceListenerList::iterator it = __instance->__selList.begin();
          it != __instance->__selList.end(); ++it)
        {
          if (sel == *it)
            {
              __instance->__selList.erase(it);
              pthread_mutex_unlock(&__instance->__selMutex);
              ILOG_DEBUG(
                  ILX_APPBASE, "SurfaceEventListener %p is removed.\n", sel);
              return true;
            }
        }

      pthread_mutex_unlock(&__instance->__selMutex);
      ILOG_ERROR(
          ILX_APPBASE, "Cannot remove SurfaceEventListener, %p not found!\n", sel);
    }
  return false;
}

void
AppBase::consumeSurfaceEvent(const DFBSurfaceEvent& event)
{
  pthread_mutex_lock(&__selMutex);

  for (SurfaceListenerList::iterator it = __selList.begin();
      it != __selList.end(); ++it)
    ((SurfaceEventListener*) *it)->consumeSurfaceEvent(event);

  pthread_mutex_unlock(&__selMutex);
}

WindowWidget*
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

      WindowList::iterator it = std::find(__instance->__windowList.begin(),
          __instance->__windowList.end(), window);
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

      for (WindowList::iterator it = __instance->__windowList.begin();
          it != __instance->__windowList.end(); ++it)
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
      ILOG_ERROR(
          ILX_APPBASE, "Cannot remove WindowWidget, %p not found!\n", window);
    }
  return false;
}

bool
AppBase::consumeWindowEvent(const DFBWindowEvent& event)
{
  return activeWindow()->handleWindowEvent((const DFBWindowEvent&) event);
}

void
AppBase::updateWindows()
{
  pthread_mutex_lock(&__windowMutex);

  for (WindowList::iterator it = __windowList.begin(); it != __windowList.end();
      ++it)
    ((WindowWidget*) *it)->updateWindow();

  pthread_mutex_unlock(&__windowMutex);
}

void
AppBase::handleEvents()
{
  DFBEvent event;
  __buffer->WaitForEventWithTimeout(__buffer, 0, 100);

  while (__buffer->GetEvent(__buffer, DFB_EVENT(&event)) == DFB_OK)
    {
      if (event.clazz == DFEC_USER)
        handleUserEvent((const DFBUserEvent&) event);
      else if (event.clazz == DFEC_WINDOW)
        {
          if (!windowPreEventFilter((const DFBWindowEvent&) event))
            if (!consumeWindowEvent((const DFBWindowEvent&) event))
              windowPostEventFilter((const DFBWindowEvent&) event);
        }
      else if (event.clazz == DFEC_SURFACE)
        consumeSurfaceEvent((const DFBSurfaceEvent&) event);
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
        ILOG_ERROR(
            ILX_APPBASE, "AttachEventBuffer error: %s!\n", DirectFBErrorString(ret));

      ret = window->_dfbWindow->RequestFocus(window->_dfbWindow);
      if (ret != DFB_OK)
        ILOG_ERROR(
            ILX_APPBASE, "RequestFocus error: %s! \n", DirectFBErrorString(ret));

      ret = window->_dfbWindow->GrabPointer(window->_dfbWindow);
      if (ret != DFB_OK)
        ILOG_ERROR(
            ILX_APPBASE, "GrabPointer error: %s!\n", DirectFBErrorString(ret));

      ret = __buffer->Reset(__buffer);
      if (ret != DFB_OK)
        ILOG_ERROR(
            ILX_APPBASE, "Buffer reset error: %s!\n", DirectFBErrorString(ret));

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
        ILOG_ERROR(
            ILX_APPBASE, "DetachEventBuffer error: %s!\n", DirectFBErrorString(ret));

      ret = window->_dfbWindow->UngrabPointer(window->_dfbWindow);
      if (ret != DFB_OK)
        ILOG_ERROR(
            ILX_APPBASE, "UngrabPointer error: %s!\n", DirectFBErrorString(ret));

      ret = __buffer->Reset(__buffer);
      if (ret != DFB_OK)
        ILOG_ERROR(ILX_APPBASE, "Buffer reset error!");

      ILOG_DEBUG(ILX_APPBASE, "Window %p is detached.\n", window);
    }
}

