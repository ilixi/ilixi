/*
 Copyright 2012 Tarik Sekmen.

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

#include "WindowWidget.h"
#include "core/AppBase.h"
#include "core/Logger.h"

using namespace ilixi;

IDirectFBSurface* WindowWidget::_exclusiveSurface = NULL;
IDirectFBSurface* WindowWidget::_cursorImage = NULL;

WindowWidget::WindowWidget(Widget* parent) :
    Frame(parent), _window(NULL), _eventManager(NULL)
{
  ILOG_DEBUG(ILX_WINDOWWIDGET, "WindowWidget\n");
  setVisible(false);
  pthread_mutex_init(&_updates._listLock, NULL);
  sem_init(&_updates._updateReady, 0, 0);
  sem_init(&_updates._paintReady, 0, 1);

  _surfaceDesc = WindowDescription;
  setMargins(5, 5, 5, 5);
  setNeighbours(this, this, this, this);

  if (AppBase::appOptions() & OptExclusive)
    {
      if (AppBase::activeWindow())
        {
          ILOG_ERROR(
              ILX_WINDOWWIDGET, "Error cannot have multiple windows in exclusive mode!\n");
          exit(1);
        }
    }
  else
    _window = new Window();

  _eventManager = new EventManager(this);
  setRootWindow(this);
}

WindowWidget::~WindowWidget()
{
  pthread_mutex_destroy(&_updates._listLock);
  sem_destroy(&_updates._updateReady);
  sem_destroy(&_updates._paintReady);

  AppBase::removeWindow(this);
  delete _eventManager;
  delete _window;
  if (AppBase::appOptions() & OptExclusive)
    {
      if (_exclusiveSurface)
        _exclusiveSurface->Release(_exclusiveSurface);
      if (_cursorImage)
        _cursorImage->Release(_cursorImage);
    }
  ILOG_TRACE_W(ILX_WINDOWWIDGET);
}

EventManager* const
WindowWidget::windowEventManager() const
{
  return _eventManager;
}

void
WindowWidget::doLayout()
{
  ILOG_TRACE_W(ILX_WINDOWWIDGET);
  update();
}

void
WindowWidget::paint(const PaintEvent& event)
{
  if (visible())
    {
      int ready;
      sem_getvalue(&_updates._updateReady, &ready);
      if (ready)
        {
          sem_wait(&_updates._updateReady);

          PaintEvent evt(_frameGeometry.intersected(_updates._updateRegion),
              PaintEvent::LeftEye);

          //          DFBRegion r = intersect.dfbRegion();
          //          _window->BeginUpdates(_window, &r);

          if (evt.isValid())
            {
              ILOG_TRACE_W(ILX_WINDOWWIDGET);
              updateSurface(evt);
#ifdef ILIXI_STEREO_OUTPUT
              // Left eye
              surface()->dfbSurface()->SetStereoEye(surface()->dfbSurface(),
                  DSSE_LEFT);
              surface()->clip(evt.rect);
              if (_backgroundFlags & BGFFill)
                {
                  surface()->clear(evt.rect);
                  compose(evt);
                }
              else if (_backgroundFlags & BGFClear)
              surface()->clear(evt.rect);

              paintChildren(evt);

              if (AppBase::appOptions() & OptExclusive)
                {
                  _exclusiveSurface->SetBlittingFlags(_exclusiveSurface,
                      DSBLIT_BLEND_ALPHACHANNEL);
                  _exclusiveSurface->Blit(_exclusiveSurface, _cursorImage, NULL,
                      AppBase::cursorPosition().x, AppBase::cursorPosition().y);
                }

              // Right eye
              surface()->dfbSurface()->SetStereoEye(surface()->dfbSurface(),
                  DSSE_RIGHT);
              evt.eye = PaintEvent::RightEye;
#endif
              surface()->clip(evt.rect);
              if (_backgroundFlags & BGFFill)
                {
                  surface()->clear(evt.rect);
                  compose(evt);
                }
              else if (_backgroundFlags & BGFClear)
                surface()->clear(evt.rect);

              paintChildren(evt);

              if (AppBase::appOptions() & OptExclusive)
                {
                  _exclusiveSurface->SetBlittingFlags(_exclusiveSurface,
                      DSBLIT_BLEND_ALPHACHANNEL);
                  _exclusiveSurface->Blit(_exclusiveSurface, _cursorImage, NULL,
                      AppBase::cursorPosition().x, AppBase::cursorPosition().y);
                }

#ifdef ILIXI_STEREO_OUTPUT
              surface()->flipStereo(evt.rect);
#else
              surface()->flip(evt.rect);
#endif
            }
          sem_post(&_updates._paintReady);
        }
      else
        {
          pthread_mutex_lock(&_updates._listLock);
          _updates._updateQueue.push_back(event.rect);
          pthread_mutex_unlock(&_updates._listLock);
        }
    }
}

void
WindowWidget::repaint(const Rectangle& rect)
{
  if (visible())
    {
      sem_wait(&_updates._paintReady);
      _updates._updateRegion = rect;
      sem_post(&_updates._updateReady);
      paint(PaintEvent(_updates._updateRegion, PaintEvent::BothEyes));
    }
}

void
WindowWidget::showWindow()
{
  ILOG_TRACE_W(ILX_WINDOWWIDGET);
  if (AppBase::appOptions() & OptExclusive)
    {
      // setup cursor
      IDirectFBImageProvider* provider;
      DFBSurfaceDescription desc;
      if (AppBase::getDFB()->CreateImageProvider(AppBase::getDFB(),
          ILIXI_DATADIR"images/pointer.png", &provider) != DFB_OK)
        ILOG_THROW(ILX_WINDOWWIDGET,
            "Error while creating cursor image provider!\n");

      provider->GetSurfaceDescription(provider, &desc);
      desc.flags = (DFBSurfaceDescriptionFlags) (DSDESC_CAPS | DSDESC_WIDTH
          | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
      desc.caps = DSCAPS_PREMULTIPLIED;
      desc.pixelformat = DSPF_ARGB;

      if (AppBase::getDFB()->CreateSurface(AppBase::getDFB(), &desc,
          &_cursorImage) != DFB_OK)
        ILOG_THROW(ILX_WINDOWWIDGET, "Error while creating cursor surface!\n");

      provider->RenderTo(provider, _cursorImage, NULL);
      provider->Release(provider);

      // setup layer
      DFBGraphicsDeviceDescription deviceDesc;
      AppBase::getDFB()->GetDeviceDescription(AppBase::getDFB(), &deviceDesc);

      DFBDisplayLayerConfig config;
      config.flags = (DFBDisplayLayerConfigFlags) (DLCONF_BUFFERMODE
          | DLCONF_WIDTH | DLCONF_HEIGHT | DLCONF_OPTIONS);
      if (deviceDesc.acceleration_mask == DFXL_NONE)
        config.buffermode = DLBM_BACKSYSTEM;
      else
        config.buffermode = DLBM_BACKVIDEO;

#ifdef ILIXI_STEREO_OUTPUT
      config.options = DLOP_STEREO;
#else
      config.options = DLOP_NONE;
#endif

      config.width = 800; //s.width();
      config.height = 600; //s.height();
      if (AppBase::__layer->SetConfiguration(AppBase::__layer, &config)
          != DFB_OK)
        ILOG_THROW(ILX_WINDOWWIDGET,
            "Error while setting layer configuration!\n");

      if (AppBase::__layer->GetSurface(AppBase::__layer, &_exclusiveSurface)
          != DFB_OK)
        ILOG_THROW(ILX_WINDOWWIDGET, "Error while getting layer surface!\n");

      int w, h;
      _exclusiveSurface->GetSize(_exclusiveSurface, &w, &h);

      AppBase::addWindow(this);
      setSize(Size(w, h));
      setRootWindow(this);
    }
  else
    {
      if (_window->_dfbWindow)
        return;

      if (_window->initDFBWindow(preferredSize()))
        {
          AppBase::addWindow(this);
          setSize(_window->windowSize());
          setRootWindow(this);
        }
    }

  setVisible(true);

  if (!_eventManager->focusedWidget())
    _eventManager->selectNeighbour(Right);

  paint(PaintEvent(Rectangle(0, 0, width(), height()), PaintEvent::BothEyes));
  updateWindow();
  if (!(AppBase::appOptions() & OptExclusive))
    _window->showWindow();

  AppBase::setActiveWindow(this);
}

void
WindowWidget::closeWindow()
{
  ILOG_TRACE_W(ILX_WINDOWWIDGET);
  setVisible(false);

  if (!(AppBase::appOptions() & OptExclusive))
    _window->hideWindow();

  _eventManager->setGrabbedWidget(NULL);
  _eventManager->setExposedWidget(NULL);

  AppBase::removeWindow(this);

  invalidateSurface();

  if (!(AppBase::appOptions() & OptExclusive))
    _window->releaseDFBWindow();
}

bool
WindowWidget::handleWindowEvent(const DFBWindowEvent& event)
{
  // handle all other events...
  Widget* target = this;
//  Widget* grabbed = _eventManager->grabbedWidget();
//  if (grabbed && grabbed->visible())
//    target = grabbed;

  switch (event.type)
    {
  case DWET_CLOSE: // handle Close, can be signalled by viewport manager.
    sigAbort();
    return true;

  case DWET_LEAVE: // handle Leave, can be signalled if pointer moves outside window.
    _eventManager->setExposedWidget(NULL,
        PointerEvent(PointerMotion, event.cx, event.cy));
    _eventManager->setGrabbedWidget(NULL,
        PointerEvent(PointerMotion, event.cx, event.cy));
    return true;

  case DWET_BUTTONUP:
    _eventManager->setGrabbedWidget(NULL,
        PointerEvent(PointerButtonUp, event.cx, event.cy, 0,
            (PointerButton) event.button, (PointerButtonMask) event.buttons));
    return target->consumePointerEvent(
        PointerEvent(PointerButtonUp, event.cx, event.cy, 0,
            (PointerButton) event.button, (PointerButtonMask) event.buttons));

  case DWET_BUTTONDOWN:
    return target->consumePointerEvent(
        PointerEvent(PointerButtonDown, event.cx, event.cy, 0,
            (PointerButton) event.button, (PointerButtonMask) event.buttons));

  case DWET_MOTION:
    return target->consumePointerEvent(
        PointerEvent(PointerMotion, event.cx, event.cy, event.step,
            (PointerButton) event.button, (PointerButtonMask) event.buttons));

  case DWET_WHEEL:
    ILOG_DEBUG(ILX_WINDOWWIDGET, "DWET_WHEEL: %d \n", event.step);
    return target->consumePointerEvent(
        PointerEvent(PointerWheel, event.cx, event.cy, event.step,
            (PointerButton) event.button, (PointerButtonMask) event.buttons));

  case DWET_KEYUP:
    if (_eventManager->focusedWidget())
      return _eventManager->focusedWidget()->consumeKeyEvent(
          KeyEvent(KeyUpEvent, event.key_symbol, event.modifiers, event.locks));
    return false;

  case DWET_KEYDOWN:
    switch (event.key_symbol)
      {

    // TODO Remove ESCAPE and sigAbort() ?
    case DIKS_ESCAPE:
      sigAbort();
      return true;

    case DIKS_BACK:
      sigAbort();
      return true;

    case DIKS_CURSOR_LEFT:
      if (target == this)
        {
          _eventManager->selectNeighbour(Left);
          return true;
        }
      break;

    case DIKS_CURSOR_RIGHT:
      if (target == this)
        {
          _eventManager->selectNeighbour(Right);
          return true;
        }
      break;

    case DIKS_CURSOR_UP:
      if (target == this)
        {
          _eventManager->selectNeighbour(Up);
          return true;
        }
      break;

    case DIKS_CURSOR_DOWN:
      if (target == this)
        {
          _eventManager->selectNeighbour(Down);
          return true;
        }
      break;
// TODO Grab using key input.
//    case DIKS_OK:
//    case DIKS_RETURN:
//      if (_eventManager->focusedWidget())
//        {
//          if (_eventManager->focusedWidget() != target)
//            _eventManager->setGrabbedWidget(_eventManager->focusedWidget());
//          else
//            _eventManager->setGrabbedWidget(NULL);
//          return true;
//        }
//      break;

    case DIKS_TAB: // handle TAB release.
      if (event.modifiers == DIMM_SHIFT)
//        _eventManager->selectPrevious();
        ILOG_DEBUG(
            ILX_WINDOWWIDGET, "TAB %d\n", _eventManager->selectPrevious());
      else
//        _eventManager->selectNext();
        ILOG_DEBUG(ILX_WINDOWWIDGET, "TAB %d\n", _eventManager->selectNext());
      return true;

      } // end switch

//    else if (_eventManager->grabbedWidget())
//      return _eventManager->grabbedWidget()->consumeKeyEvent(
//          KeyEvent(KeyDownEvent, event.key_symbol, event.modifiers,
//              event.locks));

    if (_eventManager->focusedWidget())
      return _eventManager->focusedWidget()->consumeKeyEvent(
          KeyEvent(KeyDownEvent, event.key_symbol, event.modifiers,
              event.locks));
    else
      return false;

  default:
    return false;
    }
}

void
WindowWidget::updateWindow()
{
  pthread_mutex_lock(&_updates._listLock);
  int size = _updates._updateQueue.size();
  if (size == 0)
    {
      pthread_mutex_unlock(&_updates._listLock);
      return;
    }
  ILOG_TRACE_W(ILX_WINDOWWIDGET);

  Rectangle updateTemp = _updates._updateQueue[0];
  if (size > 1)
    for (int i = 1; i < size; ++i)
      updateTemp = updateTemp.united(_updates._updateQueue[i]);

  _updates._updateQueue.clear();

  if (!updateTemp.isNull())
    {
      sem_wait(&_updates._paintReady);
#ifdef ILIXI_STEREO_OUTPUT
      _updates._updateRegion = _frameGeometry;
#else
      _updates._updateRegion = updateTemp;
#endif

      sem_post(&_updates._updateReady);
      paint(PaintEvent(_updates._updateRegion, PaintEvent::BothEyes));
    }
  pthread_mutex_unlock(&_updates._listLock);
}

IDirectFBSurface*
WindowWidget::windowSurface()
{
  if ((AppBase::appOptions() & OptExclusive))
    return _exclusiveSurface;
  else if (_window)
    return _window->dfbSurface();
  return NULL;
}
