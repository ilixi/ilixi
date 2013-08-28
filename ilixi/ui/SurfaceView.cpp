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

#include <ui/SurfaceView.h>
#include <core/Application.h>
#include <core/Logger.h>
#include <core/PlatformManager.h>
#include <ui/WindowWidget.h>
#include <sys/time.h>
#include <math.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_SURFACEVIEW, "ilixi/ui/SurfaceView", "SurfaceView");
D_DEBUG_DOMAIN( ILX_SURFACEVIEW_UPDATES, "ilixi/ui/SurfaceView/Updates", "SurfaceView");

SurfaceView::SurfaceView(Widget* parent)
        : SurfaceEventListener(),
          Widget(parent),
          _hScale(1),
          _vScale(1),
          _sourceWindow(NULL),
          _windowID(0),
          _flipCount(0),
          _svState(SV_NONE)
{
    ILOG_TRACE_W(ILX_SURFACEVIEW);
    setInputMethod(KeyPointerTracking);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &SurfaceView::onSVGeomUpdate));
#ifdef ILIXI_STEREO_OUTPUT
    _sourceStereo = false;
#endif
    _renderedSource = true;
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
    return _svState & SV_SHOULD_BLOCK;
}

bool
SurfaceView::isBlendingEnabled() const
{
    return _svState & SV_CAN_BLEND;
}

void
SurfaceView::setSourceFromSurfaceID(DFBSurfaceID sid)
{
    if (sid)
    {
        ILOG_TRACE_W(ILX_SURFACEVIEW);
        detachSourceSurface();

        DFBResult ret = PlatformManager::instance().getDFB()->GetSurface(PlatformManager::instance().getDFB(), sid, &_sourceSurface);
        if (ret)
        {
            ILOG_ERROR( ILX_SURFACEVIEW, "Error! GetSurface: %s\n", DirectFBErrorString(ret));
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
            ILOG_ERROR( ILX_SURFACEVIEW, "Error! GetSurface: %s", DirectFBErrorString(ret));
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
    if (blocking)
        _svState = (SurfaceViewFlags) (_svState | SV_SHOULD_BLOCK);
    else
        _svState = (SurfaceViewFlags) (_svState & ~SV_SHOULD_BLOCK);
}

void
SurfaceView::setBlendingEnabled(bool blending)
{
    if (blending)
        _svState = (SurfaceViewFlags) (_svState | SV_CAN_BLEND);
    else
        _svState = (SurfaceViewFlags) (_svState & ~SV_CAN_BLEND);
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
    _renderedSource = true;

    if (_sourceSurface && (_svState & SV_READY))
    {
        ILOG_TRACE_W(ILX_SURFACEVIEW);

        DFBSurfaceID surface_id;
        _sourceSurface->GetID(_sourceSurface, &surface_id);
        //D_INFO_LINE_MSG( "SURFEVT: renderSource id %d flip count %d", surface_id, _flipCount );

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
        DFBRegion rs = event.rect.dfbRegion();
        dfbSurface->SetClip(dfbSurface, &rs);

        if (opacity() == 255)
        {
            DFBSurfacePixelFormat fmt;
            _sourceSurface->GetPixelFormat(_sourceSurface, &fmt);
            if (DFB_PIXELFORMAT_HAS_ALPHA(fmt) && (_svState & SV_CAN_BLEND))
                dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_BLEND_ALPHACHANNEL);
            else
            {
                char *conv = getenv("ILIXI_COMP_CONVOLUTION");

                if (conv)
                {
                    DFBConvolutionFilter filter = { { -65536, -65536 * 2, -65536, 0, 65536, 0, 65536, 65536 * 2, 65536 }, 65536, 0 };

                    sscanf(conv, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", &filter.kernel[0], &filter.kernel[1], &filter.kernel[2], &filter.kernel[3], &filter.kernel[4], &filter.kernel[5], &filter.kernel[6], &filter.kernel[7], &filter.kernel[8], &filter.scale, &filter.bias);

                    //filter.scale += sin(direct_clock_get_millis()/1000.0) * 20000;

                    dfbSurface->SetSrcConvolution(dfbSurface, &filter);
                    dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_SRC_CONVOLUTION);
                } else
                    dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_NOFX);
            }
            dfbSurface->SetPorterDuff(dfbSurface, DSPD_SRC_OVER);
        } else
        {
            dfbSurface->SetBlittingFlags(dfbSurface, (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA));
            dfbSurface->SetPorterDuff(dfbSurface, DSPD_NONE);
            dfbSurface->SetColor(dfbSurface, 0, 0, 0, opacity());
        }

        if (hScale() == 1 && vScale() == 1)
        {
            if (surface()->flags() & Surface::SharedSurface)
                dfbSurface->Blit(dfbSurface, _sourceSurface, NULL, absX(), absY());
            else
                dfbSurface->Blit(dfbSurface, _sourceSurface, NULL, 0, 0);
        } else
        {
            DFBRectangle rect = { 0, 0, width(), height() };
            if (surface()->flags() & Surface::SharedSurface)
            {
                rect.x += absX();
                rect.y += absY();
            }
            dfbSurface->StretchBlit(dfbSurface, _sourceSurface, NULL, &rect);
        }
    }
}

bool
SurfaceView::onSourceUpdate(const DFBSurfaceEvent& event)
{
    ILOG_TRACE_W(ILX_SURFACEVIEW);

    if (!_renderedSource)
        return false;

    if (!(_svState & SV_READY))
    {
        sigSourceReady();
        _svState = (SurfaceViewFlags) (_svState | SV_READY);
    }

    if (visible())
    {
        Rectangle lRect = mapFromSurface(Rectangle(event.update.x1 / hScale(), event.update.y1 / vScale(), (event.update.x2 - event.update.x1) / hScale() + 1, (event.update.y2 - event.update.y1) / vScale() + 1));

        Application::__instance->_updateFromSurfaceView = true;
#ifdef ILIXI_STEREO_OUTPUT
        Rectangle rRect = mapFromSurface(
                Rectangle(event.update_right.x1 / hScale(), event.update_right.y1 / vScale(),
                        (event.update_right.x2 - event.update_right.x1) / hScale() + 1,
                        (event.update_right.y2 - event.update_right.y1) / vScale() + 1));

        update(PaintEvent(lRect, rRect));
#else
        update(PaintEvent(lRect));
#endif
        Application::__instance->_updateFromSurfaceView = false;
    } //else if (!(_svState & SV_SHOULD_BLOCK))

    DFBSurfaceID surface_id;
    _sourceSurface->GetID(_sourceSurface, &surface_id);
    ILOG_DEBUG(ILX_SURFACEVIEW_UPDATES, "onSourceUpdate()\n");
    ILOG_DEBUG(ILX_SURFACEVIEW_UPDATES, " -> SURFEVT: acknowledge id %d flip count %d\n", surface_id, event.flip_count);
    ILOG_DEBUG(ILX_SURFACEVIEW_UPDATES, " -> id %d\n", surface_id);
    if (surface_id == Application::__instance->_updateID)
    {
        ILOG_DEBUG(ILX_SURFACEVIEW_UPDATES, " -> update for %d\n", Application::__instance->_updateID);
        Application::__instance->_update = true;
        Application::__instance->_update_timer->restart();
    }

    _sourceSurface->FrameAck(_sourceSurface, event.flip_count);

    _renderedSource = false;
    _flipCount = event.flip_count;
    sigSourceUpdated();

    return true;
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
    _svState = SV_NONE;
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
    ILOG_TRACE_W(ILX_SURFACEVIEW);
    if (_sourceWindow)
    {
        DFBWindowEvent event;

        event.type = DWET_BUTTONDOWN;
        event.window_id = _windowID;
        event.flags = DWEF_NONE;

        event.x = (pointerEvent.x - absX()) * hScale();
        event.y = (pointerEvent.y - absY()) * vScale();

        event.cx = pointerEvent.x - parent()->x();
        event.cy = pointerEvent.y - parent()->y();

        event.button = (DFBInputDeviceButtonIdentifier) pointerEvent.button;
        event.buttons = (DFBInputDeviceButtonMask) pointerEvent.buttonMask;

        _sourceWindow->SendEvent(_sourceWindow, &event);
    }
}

void
SurfaceView::pointerButtonUpEvent(const PointerEvent& pointerEvent)
{
    ILOG_TRACE_W(ILX_SURFACEVIEW);
    if (_sourceWindow)
    {
        DFBWindowEvent event;

        event.type = DWET_BUTTONUP;
        event.window_id = _windowID;
        event.flags = DWEF_NONE;

        event.x = (pointerEvent.x - absX()) * hScale();
        event.y = (pointerEvent.y - absY()) * vScale();

        event.cx = pointerEvent.x - parent()->x();
        event.cy = pointerEvent.y - parent()->y();

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

        event.x = (pointerEvent.x - absX()) * hScale();
        event.y = (pointerEvent.y - absY()) * vScale();

        event.cx = pointerEvent.x - parent()->x();
        event.cy = pointerEvent.y - parent()->y();

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

        event.x = (pointerEvent.x - absX()) * hScale();
        event.y = (pointerEvent.y - absY()) * vScale();

        event.cx = pointerEvent.x - parent()->x();
        event.cy = pointerEvent.y - parent()->y();

        event.step = pointerEvent.wheelStep;

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

        event.x = (pointerEvent.x - absX()) * hScale();
        event.y = (pointerEvent.y - absY()) * vScale();

        event.cx = pointerEvent.x - parent()->x();
        event.cy = pointerEvent.y - parent()->y();

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

        event.x = (pointerEvent.x - absX()) * hScale();
        event.y = (pointerEvent.y - absY()) * vScale();

        event.cx = pointerEvent.x - parent()->x();
        event.cy = pointerEvent.y - parent()->y();

        event.button = (DFBInputDeviceButtonIdentifier) pointerEvent.button;
        event.buttons = (DFBInputDeviceButtonMask) pointerEvent.buttonMask;

        _sourceWindow->SendEvent(_sourceWindow, &event);
    }
}

} /* namespace ilixi */
