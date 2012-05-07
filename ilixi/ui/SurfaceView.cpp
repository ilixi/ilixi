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

#include "ui/SurfaceView.h"
#include "core/AppBase.h"
#include "ui/WindowWidget.h"
#include "core/Logger.h"
#include <sys/time.h>

namespace ilixi
{

  SurfaceView::SurfaceView(DFBSurfaceID id, Widget* parent) :
      SurfaceEventListener(id), Widget(parent), _hScale(1), _vScale(1), _sourceWindow(
          NULL), _windowID(0)
  {
    setInputMethod(KeyAndPointerInputTracking);
    setSourceFromSurfaceID(id);
    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &SurfaceView::onSVGeomUpdate));
  }

  SurfaceView::~SurfaceView()
  {
    detachEB();
    if (_sourceSurface)
      _sourceSurface->Release(_sourceSurface);
    if (_sourceWindow)
      _sourceWindow->Release(_sourceWindow);
  }

  Size
  SurfaceView::preferredSize() const
  {
    // FIXME default size for surface view.
    if (_sourceSurface)
      {
        int width, height;
        _sourceSurface->GetSize(_sourceSurface, &width, &height);
        return Size(width, height);
      }
    return Size(50, 50);
  }

  DFBSurfaceID
  SurfaceView::sourceID() const
  {
    return _surfaceID;
  }

  IDirectFBWindow*
  SurfaceView::dfbWindow() const
  {
    return _sourceWindow;
  }

  DFBWindowID
  SurfaceView::dfbWindowID() const
  {
    return _windowID;
  }

  void
  SurfaceView::setSourceFromSurfaceID(DFBSurfaceID id)
  {
    if (id)
      {
        detachEB();

        DFBResult ret = AppBase::getDFB()->GetSurface(AppBase::getDFB(), id,
            &_sourceSurface);
        if (ret)
          {
            ILOG_ERROR( ILX_SURFACEVIEW,
                "Error! GetSurface: %s", DirectFBErrorString(ret));
            _sourceSurface = NULL;
            _surfaceID = 0;
          }
        else
          {
            _sourceSurface->GetID(_sourceSurface, &_surfaceID);
            attachEB();
          }
      }
  }

  void
  SurfaceView::setSourceFromSurface(IDirectFBSurface* source)
  {
    if (source)
      {
        detachEB();

        _sourceSurface = source;
        _sourceSurface->GetID(_sourceSurface, &_surfaceID);

        attachEB();
      }
  }

  void
  SurfaceView::setSourceFromWindow(IDirectFBWindow* window)
  {
    if (window)
      {
        detachEB();

        DFBResult ret = window->GetSurface(window, &_sourceSurface);

        if (ret)
          {
            ILOG_ERROR( ILX_SURFACEVIEW,
                "Error! GetSurface: %s", DirectFBErrorString(ret));
            _sourceSurface = NULL;
            _sourceWindow = NULL;
          }
        else
          {
            _sourceWindow = window;
            _sourceWindow->AddRef(_sourceWindow);
            _sourceWindow->GetID(_sourceWindow, &_windowID);

            _sourceSurface->GetID(_sourceSurface, &_surfaceID);
            attachEB();
          }
      }
  }

  void
  SurfaceView::paint(const Rectangle& rect)
  {
    if (visible())
      {
        updateSurface();
        Rectangle intersect = _frameGeometry.intersected(rect);
        if (intersect.isValid())
          {
            compose(intersect);
            renderSource(intersect);
            paintChildren(intersect);
          }
      }
  }

  float
  SurfaceView::hScale() const
  {
    return _hScale;
  }

  float
  SurfaceView::vScale() const
  {
    return _vScale;
  }

  void
  SurfaceView::compose(const Rectangle& rect)
  {
  }

  void
  SurfaceView::renderSource(const Rectangle& rect)
  {
    if (_sourceSurface)
      {
        IDirectFBSurface* dfbSurface = surface()->DFBSurface();
        DFBRegion rs = mapToSurface(rect).dfbRegion();
        dfbSurface->SetClip(dfbSurface, &rs);

        if (opacity() >= 254)
          {
            DFBSurfacePixelFormat fmt;
            _sourceSurface->GetPixelFormat(_sourceSurface, &fmt);
            if (DFB_PIXELFORMAT_HAS_ALPHA(fmt))
              dfbSurface->SetBlittingFlags(dfbSurface,
                  DSBLIT_BLEND_ALPHACHANNEL);
            else
              dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_NOFX);
          }
        else
          {
            dfbSurface->SetBlittingFlags(dfbSurface,
                (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_ALPHACHANNEL
                    | DSBLIT_BLEND_COLORALPHA));
            dfbSurface->SetColor(dfbSurface, 0, 0, 0, opacity());
          }

        if (hScale() == 1 && vScale() == 1)
          dfbSurface->Blit(dfbSurface, _sourceSurface, NULL, 0, 0);
        else
          {
            DFBRectangle rect =
              { 0, 0, width(), height() };
            dfbSurface->StretchBlit(dfbSurface, _sourceSurface, NULL, &rect);
          }
      }
  }

  void
  SurfaceView::onSourceUpdate(const DFBSurfaceEvent& event)
  {
    if (visible())
      {
        // TODO Use rect for update.
//        Rectangle rect(event.update.x1 / hScale(), event.update.y1 / vScale(),
//            (event.update.x2 + 1 - event.update.x1) / hScale(),
//            (event.update.y2 + 1 - event.update.y1) / vScale());
//
//        update(mapFromSurface(rect));
        update();
      }
  }

  void
  SurfaceView::onSourceDestroyed(const DFBSurfaceEvent& event)
  {
    if (_sourceSurface)
      _sourceSurface->Release(_sourceSurface);
    if (_sourceWindow)
      _sourceWindow->Release(_sourceWindow);
    _sourceSurface = NULL;
    _sourceWindow = NULL;
    _surfaceID = 0;
  }

  void
  SurfaceView::onSVGeomUpdate()
  {
    if (_sourceSurface)
      {
        int w, h;
        _sourceSurface->GetSize(_sourceSurface, &w, &h);
        _hScale = (float) (0.0 + w) / width();
        _vScale = (float) (0.0 + h) / height();
      }
  }

  void
  SurfaceView::keyDownEvent(const KeyEvent& keyEvent)
  {
    if (_sourceWindow)
      {
        DFBWindowEvent event;

        event.type = DWET_KEYDOWN;

        event.key_symbol = keyEvent.keySymbol;
        event.modifiers = keyEvent.modifierMask;
        event.locks = keyEvent.lockState;

        _sourceWindow->SendEvent(_sourceWindow, &event);
      }
  }

  void
  SurfaceView::keyUpEvent(const KeyEvent& keyEvent)
  {
    if (_sourceWindow)
      {
        DFBWindowEvent event;

        event.type = DWET_KEYUP;

        event.key_symbol = keyEvent.keySymbol;
        event.modifiers = keyEvent.modifierMask;
        event.locks = keyEvent.lockState;

        _sourceWindow->SendEvent(_sourceWindow, &event);
      }
  }

  void
  SurfaceView::pointerButtonDownEvent(const PointerEvent& pointerEvent)
  {
    if (_sourceWindow)
      {
        DFBWindowEvent event;

        event.type = DWET_BUTTONDOWN;

        event.x = event.cx = pointerEvent.x * hScale();
        event.y = event.cy = pointerEvent.y * vScale();

        event.button = (DFBInputDeviceButtonIdentifier) pointerEvent.button;
        event.buttons = (DFBInputDeviceButtonMask) pointerEvent.buttonMask;

        _sourceWindow->SendEvent(_sourceWindow, &event);
      }
  }

  void
  SurfaceView::pointerButtonUpEvent(const PointerEvent& pointerEvent)
  {
    if (_sourceWindow)
      {
        DFBWindowEvent event;

        event.type = DWET_BUTTONUP;

        event.x = event.cx = pointerEvent.x * hScale();
        event.y = event.cy = pointerEvent.y * vScale();

        event.button = (DFBInputDeviceButtonIdentifier) pointerEvent.button;
        event.buttons = (DFBInputDeviceButtonMask) pointerEvent.buttonMask;

        _sourceWindow->SendEvent(_sourceWindow, &event);
      }
  }

  void
  SurfaceView::pointerMotionEvent(const PointerEvent& pointerEvent)
  {
    if (_sourceWindow)
      {
        DFBWindowEvent event;

        event.type = DWET_MOTION;

        event.x = event.cx = pointerEvent.x * hScale();
        event.y = event.cy = pointerEvent.y * vScale();

        event.button = (DFBInputDeviceButtonIdentifier) pointerEvent.button;
        event.buttons = (DFBInputDeviceButtonMask) pointerEvent.buttonMask;

        _sourceWindow->SendEvent(_sourceWindow, &event);
      }
  }

  void
  SurfaceView::pointerWheelEvent(const PointerEvent& pointerEvent)
  {
    if (_sourceWindow)
      {
        DFBWindowEvent event;

        event.type = DWET_WHEEL;

        event.x = event.cx = pointerEvent.x * hScale();
        event.y = event.cy = pointerEvent.y * vScale();

        event.button = (DFBInputDeviceButtonIdentifier) pointerEvent.button;
        event.buttons = (DFBInputDeviceButtonMask) pointerEvent.buttonMask;

        _sourceWindow->SendEvent(_sourceWindow, &event);
      }
  }

  void
  SurfaceView::focusInEvent()
  {
    if (_sourceWindow)
      {
        DFBWindowEvent event;

        event.type = DWET_GOTFOCUS;

        _sourceWindow->SendEvent(_sourceWindow, &event);
      }
  }

  void
  SurfaceView::focusOutEvent()
  {
    if (_sourceWindow)
      {
        DFBWindowEvent event;

        event.type = DWET_LOSTFOCUS;

        _sourceWindow->SendEvent(_sourceWindow, &event);
      }
  }

  void
  SurfaceView::enterEvent(const PointerEvent& pointerEvent)
  {
    if (_sourceWindow)
      {
        DFBWindowEvent event;

        event.type = DWET_ENTER;

        event.x = event.cx = pointerEvent.x * hScale();
        event.y = event.cy = pointerEvent.y * vScale();

        event.button = (DFBInputDeviceButtonIdentifier) pointerEvent.button;
        event.buttons = (DFBInputDeviceButtonMask) pointerEvent.buttonMask;

        _sourceWindow->SendEvent(_sourceWindow, &event);
      }
  }

  void
  SurfaceView::leaveEvent(const PointerEvent& pointerEvent)
  {
    if (_sourceWindow)
      {
        DFBWindowEvent event;

        event.type = DWET_LEAVE;

        event.x = event.cx = pointerEvent.x * hScale();
        event.y = event.cy = pointerEvent.y * vScale();

        event.button = (DFBInputDeviceButtonIdentifier) pointerEvent.button;
        event.buttons = (DFBInputDeviceButtonMask) pointerEvent.buttonMask;

        _sourceWindow->SendEvent(_sourceWindow, &event);
      }
  }

} /* namespace ilixi */
