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

using namespace ilixi;

Window::CallbackList Window::__callbacks;
IDirectFB* Window::__dfb = NULL;
IDirectFBDisplayLayer* Window::__layer = NULL;
IDirectFBEventBuffer* Window::__buffer = NULL;
Window::WindowList Window::__windowList;
Window* Window::__activeWindow = NULL;
pthread_mutex_t Window::__windowMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Window::__cbMutex = PTHREAD_MUTEX_INITIALIZER;

Window::Window() :
  _window(NULL), _windowSurface(NULL), _eventManager(NULL), _parentWindow(NULL)//, _windowType(CustomWindow)
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
      _dialogShow->start();
//            _window->SetOpacity(_window, 255);
    }
  else
    _window->SetOpacity(_window, 255);

  _window->AttachEventBuffer(_window, __buffer);
  _window->RequestFocus(_window);
  __buffer->Reset(__buffer);

  pthread_mutex_unlock(&__windowMutex);
  ILOG_INFO("Window is visible.");
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
  ILOG_INFO("Window is hidden.");
}

bool
Window::initDFB(int argc, char **argv)
{
  if (!__dfb)
    {
      if (DirectFBInit(&argc, &argv) != DFB_OK)
        {
          ILOG_ERROR("Error while initialising DirectFB!");
          return false;
        }
      if (DirectFBCreate(&__dfb) != DFB_OK)
        {
          ILOG_ERROR("Error while creating DirectFB interface!");
          return false;
        }
      if (__dfb->CreateEventBuffer(__dfb, &__buffer) != DFB_OK)
        {
          ILOG_ERROR("Error while creating event buffer!");
          return false;
        }
      if (__dfb->GetDisplayLayer(__dfb, DLID_PRIMARY, &__layer) != DFB_OK)
        {
          ILOG_ERROR("Error while getting primary layer!");
          return false;
        }
      ILOG_INFO("DirectFB interface is ready.");
      return true;
    }
  return false;
}

bool
Window::initDFBWindow(const Size& size)
{
  if (!__dfb)
    {
      ILOG_FATAL("DirectFB interface is not initialised!");
      exit(EXIT_FAILURE);
    }

  DFBWindowDescription desc;
  desc.flags = (DFBWindowDescriptionFlags) (DWDESC_POSX | DWDESC_POSY
      | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_SURFACE_CAPS
      | DWDESC_PIXELFORMAT | DWDESC_OPTIONS | DWDESC_STACKING);

  desc.pixelformat = DSPF_ARGB;
  desc.caps = (DFBWindowCapabilities) (DWCAPS_DOUBLEBUFFER
      | DWCAPS_ALPHACHANNEL); // DWCAPS_NODECORATION
  desc.surface_caps = DSCAPS_DOUBLE;
  desc.options = (DFBWindowOptions) (DWOP_ALPHACHANNEL | DWOP_SCALE);

  DFBDisplayLayerConfig conf;
  if (__layer->GetConfiguration(__layer, &conf) != DFB_OK)
    {
      ILOG_ERROR("Error while getting primary layer configuration!");
      return false;
    }

  if (!__activeWindow)
    {
      ILOG_DEBUG("Main window is initialising...");
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
      desc.stacking = DWSC_LOWER;
    }
  else // other windows
    {
      ILOG_DEBUG("Window is initialising...");
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
      desc.stacking = DWSC_MIDDLE;
    }

  if (__layer->CreateWindow(__layer, &desc, &_window) != DFB_OK)
    {
      ILOG_ERROR("Error while creating DirectFB window!");
      return false;
    }

  if (_window->GetSurface(_window, &_windowSurface) != DFB_OK)
    {
      ILOG_ERROR("Unable to acquire surface from application window.");
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

  ILOG_INFO("Window is initialised.");
  return true;
}

void
Window::releaseDFBWindow()
{
  if (_window)
    {
      ILOG_DEBUG("releaseWindow...");
      if (_parentWindow)
        {
          ILOG_DEBUG( "Releasing DirectFB window interface...");
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
          ILOG_DEBUG( "DirectFB window interface is released.");
        }
      else
        {
          ILOG_DEBUG( "Releasing DirectFB application window interface...");
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
          ILOG_DEBUG( "DirectFB application window is released.");
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
  ILOG_DEBUG("Releasing DirectFB interfaces...");
  if (__dfb)
    {
      __buffer->Release(__buffer);
      __layer->Release(__layer);
      __dfb->Release(__dfb);
      __dfb = NULL;
      __activeWindow = NULL;
      pthread_mutex_destroy(&__windowMutex);
      ILOG_DEBUG("DirectFB interface is released.");
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
      for (CallbackList::iterator it = __callbacks.begin(); it
          != __callbacks.end(); ++it)
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

