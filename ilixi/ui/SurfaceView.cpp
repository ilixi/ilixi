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

#include <ui/SurfaceView.h>
#include <core/AppBase.h>
#include <ui/WindowWidget.h>
#include <core/Logger.h>
#include <sys/time.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_SURFACEVIEW, "ilixi/ui/SurfaceView", "SurfaceView");

SurfaceView::SurfaceView(Widget* parent)
        : SurfaceEventListener(),
          Widget(parent),
          _blocking(true),
          _hScale(1),
          _vScale(1),
          _sourceWindow(NULL),
          _windowID(0),
          _flipCount(0),
          _state(SVS_NONE)
{
    ILOG_TRACE_W(ILX_SURFACEVIEW);
    setInputMethod(KeyAndPointerInputTracking);
    sigGeometryUpdated.connect(
            sigc::mem_fun(this, &SurfaceView::onSVGeomUpdate));
#ifdef ILIXI_STEREO_OUTPUT
    _sourceStereo = false;
#endif
}

SurfaceView::~SurfaceView()
{
    detachSourceSurface();
    if (_sourceSurface)
        _sourceSurface->Release(_sourceSurface);
    if (_sourceWindow)
        _sourceWindow->Release(_sourceWindow);
    ILOG_TRACE_W(ILX_SURFACEVIEW);
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

bool
SurfaceView::isBlocking() const
{
    return _blocking;
}

void
SurfaceView::setSourceFromSurfaceID(DFBSurfaceID sid)
{
    if (sid)
    {
        ILOG_TRACE_W(ILX_SURFACEVIEW);
        detachSourceSurface();

        DFBResult ret = AppBase::getDFB()->GetSurface(AppBase::getDFB(), sid,
                                                      &_sourceSurface);
        if (ret)
        {
            ILOG_ERROR( ILX_SURFACEVIEW,
                       "Error! GetSurface: %s\n", DirectFBErrorString(ret));
            _sourceSurface = NULL;
            _surfaceID = 0;
        } else
        {
            _sourceSurface->GetID(_sourceSurface, &_surfaceID);

#ifdef ILIXI_STEREO_OUTPUT
            DFBSurfaceCapabilities caps;
            _sourceSurface->GetCapabilities(_sourceSurface, &caps);
            if (caps & DSCAPS_STEREO)
            _sourceStereo = true;
#endif

            attachSourceSurface();
        }
    }
}

void
SurfaceView::setSourceFromSurface(IDirectFBSurface* source)
{
    if (source)
    {
        ILOG_TRACE_W(ILX_SURFACEVIEW);
        detachSourceSurface();

        _sourceSurface = source;
        _sourceSurface->GetID(_sourceSurface, &_surfaceID);

#ifdef ILIXI_STEREO_OUTPUT
        DFBSurfaceCapabilities caps;
        _sourceSurface->GetCapabilities(_sourceSurface, &caps);
        if (caps & DSCAPS_STEREO)
        _sourceStereo = true;
#endif

        attachSourceSurface();
    }
}

void
SurfaceView::setSourceFromWindow(IDirectFBWindow* window)
{
    if (window)
    {
        ILOG_TRACE_W(ILX_SURFACEVIEW);
        detachSourceSurface();

        DFBResult ret = window->GetSurface(window, &_sourceSurface);

        if (ret)
        {
            ILOG_ERROR( ILX_SURFACEVIEW,
                       "Error! GetSurface: %s", DirectFBErrorString(ret));
            _sourceSurface = NULL;
            _sourceWindow = NULL;
        } else
        {
            _sourceWindow = window;
            _sourceWindow->AddRef(_sourceWindow);
            _sourceWindow->GetID(_sourceWindow, &_windowID);

            _sourceSurface->GetID(_sourceSurface, &_surfaceID);

#ifdef ILIXI_STEREO_OUTPUT
            DFBSurfaceCapabilities caps;
            _sourceSurface->GetCapabilities(_sourceSurface, &caps);
            if (caps & DSCAPS_STEREO)
            _sourceStereo = true;
#endif
            attachSourceSurface();
        }
    }
}

void
SurfaceView::setBlocking(bool blocking)
{
    _blocking = blocking;
    ILOG_TRACE_W(ILX_SURFACEVIEW);
    ILOG_DEBUG(ILX_SURFACEVIEW, " -> Blocking: %d\n", _blocking);
}

void
SurfaceView::paint(const PaintEvent& event)
{
    if (visible())
    {
        PaintEvent evt(this, event);
        if (evt.isValid())
        {
            compose(evt);
            renderSource(evt);
            paintChildren(evt);
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
SurfaceView::compose(const PaintEvent& event)
{
}

void
SurfaceView::renderSource(const PaintEvent& event)
{
    if (_sourceSurface && _state == SVS_READY)
    {
        ILOG_TRACE_W(ILX_SURFACEVIEW);
#ifdef ILIXI_STEREO_OUTPUT
        if (_flipCount && event.eye == PaintEvent::RightEye)
#else
        if (_flipCount)
#endif
        {
            _sourceSurface->FrameAck(_sourceSurface, _flipCount);
            _flipCount = 0;
        }

#ifdef ILIXI_STEREO_OUTPUT
        if (_sourceStereo)
        {
            if (event.eye == PaintEvent::LeftEye)
            _sourceSurface->SetStereoEye(_sourceSurface, DSSE_LEFT);
            else
            _sourceSurface->SetStereoEye(_sourceSurface, DSSE_RIGHT);
        }
#endif

        IDirectFBSurface* dfbSurface = surface()->dfbSurface();
        DFBRegion rs = mapToSurface(event.rect).dfbRegion();
        dfbSurface->SetClip(dfbSurface, &rs);

        if (opacity() == 255)
        {
            DFBSurfacePixelFormat fmt;
            _sourceSurface->GetPixelFormat(_sourceSurface, &fmt);
            //if (DFB_PIXELFORMAT_HAS_ALPHA(fmt))
            //    dfbSurface->SetBlittingFlags(dfbSurface,
            //                                 DSBLIT_BLEND_ALPHACHANNEL);
            //else
                dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_NOFX);
        } else
        {
            dfbSurface->SetBlittingFlags(
                    dfbSurface,
                    (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_ALPHACHANNEL
                            | DSBLIT_BLEND_COLORALPHA));
            dfbSurface->SetColor(dfbSurface, 0, 0, 0, opacity());
        }

        if (hScale() == 1 && vScale() == 1)
            dfbSurface->Blit(dfbSurface, _sourceSurface, NULL, 0, 0);
        else
        {
            DFBRectangle rect = { 0, 0, width(), height() };
            dfbSurface->StretchBlit(dfbSurface, _sourceSurface, NULL, &rect);
        }
    }
}

void
SurfaceView::onSourceUpdate(const DFBSurfaceEvent& event)
{
    if (_state != SVS_READY)
    {
        sigSourceReady();
        _state = SVS_READY;
    }

    if (visible())
    {
        Rectangle lRect = mapFromSurface(
                Rectangle(event.update.x1 / hScale(),
                          event.update.y1 / vScale(),
                          (event.update.x2 - event.update.x1) / hScale() + 1,
                          (event.update.y2 - event.update.y1) / vScale() + 1));

#ifdef ILIXI_STEREO_OUTPUT
        Rectangle rRect = mapFromSurface(
                Rectangle(event.update_right.x1 / hScale(), event.update_right.y1 / vScale(),
                        (event.update_right.x2 - event.update_right.x1) / hScale() + 1,
                        (event.update_right.y2 - event.update_right.y1) / vScale() + 1));

        update(PaintEvent(lRect, rRect));
#else
        update(PaintEvent(lRect));
#endif
    } else if (!_blocking)
        _sourceSurface->FrameAck(_sourceSurface, event.flip_count);

    _flipCount = event.flip_count;
    sigSourceUpdated();
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
    _state = SVS_NONE;
    sigSourceDestroyed();
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
        event.window_id = _windowID;
        event.flags = DWEF_NONE;

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
        event.window_id = _windowID;
        event.flags = DWEF_NONE;

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
        event.window_id = _windowID;
        event.flags = DWEF_NONE;

        event.x = pointerEvent.x * hScale() - absX();
        event.y = pointerEvent.y * vScale() - absY();

        event.cx = pointerEvent.x * hScale();
        event.cy = pointerEvent.y * vScale();

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
        event.window_id = _windowID;
        event.flags = DWEF_NONE;

        event.x = pointerEvent.x * hScale() - absX();
        event.y = pointerEvent.y * vScale() - absY();

        event.cx = pointerEvent.x * hScale();
        event.cy = pointerEvent.y * vScale();

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
        event.window_id = _windowID;
        event.flags = DWEF_NONE;

        event.x = pointerEvent.x * hScale() - absX();
        event.y = pointerEvent.y * vScale() - absY();

        event.cx = pointerEvent.x * hScale();
        event.cy = pointerEvent.y * vScale();

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
        event.window_id = _windowID;
        event.flags = DWEF_NONE;

        event.x = pointerEvent.x * hScale() - absX();
        event.y = pointerEvent.y * vScale() - absY();

        event.cx = pointerEvent.x * hScale();
        event.cy = pointerEvent.y * vScale();

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

        event.flags = DWEF_NONE;
        event.type = DWET_GOTFOCUS;
        event.window_id = _windowID;

        _sourceWindow->SendEvent(_sourceWindow, &event);
    }
}

void
SurfaceView::focusOutEvent()
{
    if (_sourceWindow)
    {
        DFBWindowEvent event;

        event.flags = DWEF_NONE;
        event.type = DWET_LOSTFOCUS;
        event.window_id = _windowID;

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
        event.window_id = _windowID;
        event.flags = DWEF_NONE;

        event.x = pointerEvent.x * hScale() - absX();
        event.y = pointerEvent.y * vScale() - absY();

        event.cx = pointerEvent.x * hScale();
        event.cy = pointerEvent.y * vScale();

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
        event.window_id = _windowID;
        event.flags = DWEF_NONE;

        event.x = pointerEvent.x * hScale() - absX();
        event.y = pointerEvent.y * vScale() - absY();

        event.cx = pointerEvent.x * hScale();
        event.cy = pointerEvent.y * vScale();

        event.button = (DFBInputDeviceButtonIdentifier) pointerEvent.button;
        event.buttons = (DFBInputDeviceButtonMask) pointerEvent.buttonMask;

        _sourceWindow->SendEvent(_sourceWindow, &event);
    }
}

} /* namespace ilixi */
