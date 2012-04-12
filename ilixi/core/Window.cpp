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

#include "core/Window.h"
#include "core/AppBase.h"
#include "core/IFusion.h"
#include "core/Logger.h"
#include <algorithm>

D_DEBUG_DOMAIN( ILX_WINDOW, "ilixi/core/Window", "Window");

using namespace ilixi;

Window::CallbackList Window::__callbacks;
Window::SurfaceListenerList Window::__selList;
IDirectFB* Window::__dfb = NULL;
IDirectFBDisplayLayer* Window::__layer = NULL;
IDirectFBEventBuffer* Window::__buffer = NULL;
Window::WindowList Window::__windowList;
Window* Window::__activeWindow = NULL;
pthread_mutex_t Window::__windowMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t Window::__cbMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t Window::__selMutex = PTHREAD_MUTEX_INITIALIZER;

    Window::Window() :
    _window(NULL), _windowSurface ( NULL
), _eventManager(NULL), _parentWindow(NULL)
  {
    _eventManager = new EventManager(this);

    _dialogShow = new TweenAnimation();
    _dialogShow->addTween(Tween::LINEAR, Tween::EASE_IN, _aniValue, 0, 255);
    _dialogShow->addTween(Tween::ELASTIC, Tween::EASE_OUT, _aniValue2, 0, 1);
    _dialogShow->setDuration(500);
    _dialogShow->sigExec.connect(sigc::mem_fun(this, &Window::animateWindow));

//  _dialogHide = new TweenAnimation();
//  _dialogHide->addTween(Tween::SINE, Tween::EASE_OUT, _aniValue, 1, 0);
//  _dialogHide->setDuration(500);
//  _dialogHide->sigExec.connect(sigc::mem_fun(this, &Window::animateWindow));
  }

Window::~Window()
{
  ILOG_DEBUG(ILX_WINDOW, "~Window\n");
  delete _dialogShow;
  //  delete _dialogHide;
  delete _eventManager;
  releaseDFBWindow();
  if (!_parentWindow)
    releaseDFB();
}

DFBWindowID
Window::getWindowID() const
{
  DFBWindowID windowID;
  _window->GetID(_window, &windowID);
  return windowID;
}

IDirectFBSurface*
Window::getDFBSurface() const
{
  return _windowSurface;
}

Size
Window::getWindowSize() const
{
  int w, h;
  _window->GetSize(_window, &w, &h);
  return Size(w, h);
}

void
Window::showWindow()
{
  if (!_window)
    return;

  pthread_mutex_lock(&__windowMutex);
  if (__activeWindow)
    {
      __activeWindow->_eventManager->setGrabbedWidget(NULL);
      __activeWindow->_eventManager->setExposedWidget(NULL);
    }

  __activeWindow = this;

  // Try to set focus, do not change if it is already set...
  if (!__activeWindow->_eventManager->focusedWidget())
    __activeWindow->_eventManager->selectNext(true);

  _window->RaiseToTop(_window);

  if (_parentWindow)
    {
      _parentWindow->_window->DetachEventBuffer(_parentWindow->_window,
          __buffer);
      // animation goes here for other window types...
      _window->GrabPointer(_window);
//      _dialogShow->start();
      _window->SetOpacity(_window, 255);
    }
  else
    _window->SetOpacity(_window, 255);

  _window->AttachEventBuffer(_window, __buffer);
  _window->RequestFocus(_window);
  __buffer->Reset(__buffer);

  pthread_mutex_unlock(&__windowMutex);
  ILOG_INFO(ILX_WINDOW, "Window is visible.\n");
}

void
Window::hideWindow()
{
  if (!_window)
    return;

  pthread_mutex_lock(&__windowMutex);

  _window->DetachEventBuffer(_window, __buffer);
  __buffer->Reset(__buffer);

  if (_parentWindow)
    {
      // animation goes here for other window types...
      _window->UngrabPointer(_window);
      _window->SetOpacity(_window, 0);
      //      _dialogShow->stop();
      //      _dialogHide->start();

      __activeWindow = _parentWindow;
      _parentWindow->_window->AttachEventBuffer(_parentWindow->_window,
          __buffer);
      _parentWindow->_window->RequestFocus(_parentWindow->_window);
    }
  else
    _window->SetOpacity(_window, 0);

  _window->LowerToBottom(_window);

  _eventManager->setGrabbedWidget(NULL);
  _eventManager->setExposedWidget(NULL);

  pthread_mutex_unlock(&__windowMutex);
  ILOG_INFO(ILX_WINDOW, "Window is hidden.\n");
}

bool
Window::initDFB(int argc, char **argv, AppOptions opts)
{
  if (!__dfb)
    {
      if (DirectFBInit(&argc, &argv) != DFB_OK)
        {
          ILOG_ERROR(ILX_WINDOW, "Error while initialising DirectFB!\n");
          return false;
        }
      if (DirectFBCreate(&__dfb) != DFB_OK)
        {
          ILOG_ERROR(ILX_WINDOW, "Error while creating DirectFB interface!\n");
          return false;
        }
      if (__dfb->CreateEventBuffer(__dfb, &__buffer) != DFB_OK)
        {
          ILOG_ERROR(ILX_WINDOW, "Error while creating event buffer!\n");
          return false;
        }
      if (__dfb->GetDisplayLayer(__dfb, DLID_PRIMARY, &__layer) != DFB_OK)
        {
          ILOG_ERROR(ILX_WINDOW, "Error while getting primary layer!\n");
          return false;
        }

      if (opts & OptCompositor)
        {
          if (__layer->SetCooperativeLevel(__layer, DLSCL_ADMINISTRATIVE))
            {
              ILOG_ERROR(ILX_WINDOW, "Error while setting EXLUSIVE mode!");
              return false;
            }
        }

      ILOG_INFO(ILX_WINDOW, "DirectFB interface is ready.\n");
      return true;
    }
  return false;
}

bool
Window::initDFBWindow(const Size& size)
{
  if (!__dfb)
    {
      ILOG_FATAL(ILX_WINDOW, "DirectFB interface is not initialised!\n");
      exit(EXIT_FAILURE);
    }
  DFBResult ret;
  DFBWindowDescription desc;
  desc.flags = (DFBWindowDescriptionFlags) (DWDESC_POSX | DWDESC_POSY
      | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_SURFACE_CAPS
      | DWDESC_PIXELFORMAT | DWDESC_OPTIONS | DWDESC_STACKING);

  desc.pixelformat = DSPF_ARGB;
  desc.caps =
      (DFBWindowCapabilities) (DWCAPS_DOUBLEBUFFER | DWCAPS_ALPHACHANNEL); // DWCAPS_NODECORATION
  desc.surface_caps = DSCAPS_DOUBLE;
  desc.options = (DFBWindowOptions) (DWOP_ALPHACHANNEL | DWOP_SCALE);

  DFBDisplayLayerConfig conf;
  ret = __layer->GetConfiguration(__layer, &conf);
  if (ret != DFB_OK)
    {
      ILOG_ERROR( ILX_WINDOW,
          "Error while getting primary layer configuration (%s)!\n", DirectFBErrorString(ret));
      return false;
    }

  if (!__activeWindow)
    {
      ILOG_DEBUG(ILX_WINDOW, "Main window is initialising...\n");
#if ILIXI_MULTI_ENABLED
      // TODO Bounds of windows will be set by a WM when a window is added to stack, e.g. SaWMan.
      if (AppBase::__appInstance->__appRecord->fusionID > 2)
        {
          desc.posx = 0;
          desc.posy = 50;
          desc.width = conf.width;
          desc.height = conf.height - 50;
        }
      else
        {
          desc.posx = 0;
          desc.posy = 0;
          desc.width = conf.width;
          desc.height = 50;
        }
#else
      desc.posx = 0;
      desc.posy = 0;
      desc.width = conf.width;
      desc.height = conf.height;
#endif
      desc.stacking = DWSC_MIDDLE;
    }
  else // other windows
    {
      ILOG_DEBUG(ILX_WINDOW, "Window is initialising...\n");
      int w = size.width();
      int h = size.height();
      if (w > (conf.width - 20))
        w = conf.width - 20;

      if (h > (conf.height - 20))
        h = conf.height - 20;

      int x = (conf.width - w) / 2.0;
      int y = (conf.height - h) / 2.0;

      _bounds.setRectangle(x, y, w, h);

      desc.posx = x;
      desc.posy = y;
      desc.width = w;
      desc.height = h;
      desc.stacking = DWSC_UPPER;
    }

  ret = __layer->CreateWindow(__layer, &desc, &_window);
  if (ret != DFB_OK)
    {
      ILOG_ERROR(ILX_WINDOW,
          "Error while creating DirectFB window! (%s)!\n", DirectFBErrorString(ret));
      return false;
    }

  ret = _window->GetSurface(_window, &_windowSurface);
  if (ret != DFB_OK)
    {
      ILOG_ERROR( ILX_WINDOW,
          "Unable to acquire surface from application window. (%s)!\n", DirectFBErrorString(ret));
      return false;
    }
#if ILIXI_MULTI_ENABLED
  _window->SetApplicationID(_window,
      AppBase::__appInstance->__appRecord->fusionID);
#endif
  pthread_mutex_lock(&__windowMutex);
  _parentWindow = __activeWindow;
  __windowList.push_back(this);
  pthread_mutex_unlock(&__windowMutex);

  ILOG_INFO(ILX_WINDOW, "Window is initialised.\n");
  return true;
}

void
Window::releaseDFBWindow()
{
  if (_window)
    {
      ILOG_DEBUG(ILX_WINDOW, "releaseWindow...\n");
      if (_parentWindow)
        {
          ILOG_DEBUG(ILX_WINDOW, "Releasing DirectFB window interface...\n");
          pthread_mutex_lock(&__windowMutex);

          _windowSurface->Release(_windowSurface);
          _window->Close(_window);
          _window->Destroy(_window);
          _window->Release(_window);
          _window = NULL;

          WindowList::iterator it = std::find(__windowList.begin(),
              __windowList.end(), this);
          if (this == *it)
            __windowList.erase(it);

          pthread_mutex_unlock(&__windowMutex);
          ILOG_DEBUG(ILX_WINDOW, "DirectFB window interface is released.\n");
        }
      else
        {
          ILOG_DEBUG( ILX_WINDOW,
              "Releasing DirectFB application window interface...\n");
          pthread_mutex_lock(&__windowMutex);

          _windowSurface->Release(_windowSurface);
          _window->Close(_window);
          _window->Destroy(_window);
          _window->Release(_window);
          _window = NULL;
          __activeWindow = NULL;

          WindowList::iterator it = std::find(__windowList.begin(),
              __windowList.end(), this);
          if (this == *it)
            __windowList.erase(it);

          pthread_mutex_unlock(&__windowMutex);
          ILOG_DEBUG( ILX_WINDOW, "DirectFB application window is released.\n");
        }
    }
}

void
Window::animateWindow()
{
  _window->SetOpacity(_window, _aniValue);
  _window->MoveTo(_window, _aniValue2 * _bounds.x(), _bounds.y());
}

IDirectFB*
Window::getDFB()
{
  return __dfb;
}

void
Window::releaseDFB()
{
  ILOG_DEBUG(ILX_WINDOW, "Releasing DirectFB interfaces...\n");
  if (__dfb)
    {
      __buffer->Release(__buffer);
      __layer->Release(__layer);
      __dfb->Release(__dfb);
      __dfb = NULL;
      __activeWindow = NULL;
      pthread_mutex_destroy(&__windowMutex);
      ILOG_DEBUG(ILX_WINDOW, "DirectFB interface is released.\n");
    }
}

Window*
Window::activeWindow()
{
  pthread_mutex_lock(&__windowMutex);
  Window* w = __activeWindow;
  pthread_mutex_unlock(&__windowMutex);
  return w;
}

void
Window::addCallback(Callback* cb)
{
  if (cb)
    {
      pthread_mutex_lock(&__cbMutex);
      __callbacks.push_back(cb);
      pthread_mutex_unlock(&__cbMutex);
    }
}

bool
Window::removeCallback(Callback* cb)
{
  if (cb)
    {
      pthread_mutex_lock(&__cbMutex);
      for (CallbackList::iterator it = __callbacks.begin();
          it != __callbacks.end(); ++it)
        {
          if (cb == *it)
            {
              __callbacks.erase(it);
              pthread_mutex_unlock(&__cbMutex);
              return true;
            }
        }

      pthread_mutex_unlock(&__cbMutex);
    }
  return false;
}

bool
Window::addSurfaceEventListener(SurfaceEventListener* sel)
{
  if (sel)
    {
      pthread_mutex_lock(&__selMutex);
      __selList.push_back(sel);
      pthread_mutex_unlock(&__selMutex);
    }
}

bool
Window::removeSurfaceEventListener(SurfaceEventListener* sel)
{
  if (sel)
    {
      pthread_mutex_lock(&__selMutex);
      for (SurfaceListenerList::iterator it = __selList.begin();
          it != __selList.end(); ++it)
        {
          if (sel)
            {
              __selList.erase(it);
              pthread_mutex_unlock(&__selMutex);
              return true;
            }
        }

      pthread_mutex_unlock(&__selMutex);
    }
  return false;
}

void
Window::handleSurfaceEvent(const DFBSurfaceEvent& event)
{
  pthread_mutex_lock(&__selMutex);
  for (SurfaceListenerList::iterator it = __selList.begin();
      it != __selList.end(); ++it)
    ((SurfaceEventListener*) *it)->consumeSurfaceEvent(event);
  pthread_mutex_unlock(&__selMutex);
}

void
Window::handleCallbacks()
{
  pthread_mutex_lock(&__cbMutex);
  for (CallbackList::iterator it = __callbacks.begin(); it != __callbacks.end();
      ++it)
    ((Callback*) *it)->_func(((Callback*) *it)->_data);
  if (!__callbacks.empty())
    __buffer->WakeUp(__buffer);
  pthread_mutex_unlock(&__cbMutex);
}

