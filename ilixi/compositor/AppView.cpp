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

#include <compositor/AppView.h>
#include <compositor/Compositor.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_APPVIEW, "ilixi/compositor/AppView", "AppView");

AppView::AppView(ILXCompositor* compositor, AppInstance* instance, Widget* parent)
        : AppCompositor(compositor, instance, parent),
          _animProps((AnimatedProperty) (Opacity))
{
    setInputMethod(PointerPassthrough);

    _propAnim.setDuration(_compositor->settings.durationShow);
    _propAnim.sigExec.connect(sigc::mem_fun(this, &AppView::tweenSlot));
    _propAnim.sigFinished.connect(sigc::mem_fun(this, &AppView::tweenEndSlot));

    _opacityTween = new Tween(Tween::SINE, Tween::EASE_IN, 0, 255);
    _propAnim.addTween(_opacityTween);

    _zoomTween = new Tween(Tween::QUAD, Tween::EASE_IN, 0.8, 1);
    _propAnim.addTween(_zoomTween);

    _xTween = new Tween(Tween::CUBIC, Tween::EASE_OUT, 0, 0);
    _propAnim.addTween(_xTween);

    _yTween = new Tween(Tween::CUBIC, Tween::EASE_OUT, 0, 0);
    _propAnim.addTween(_yTween);

    setVisible(false);
    ILOG_TRACE_W(ILX_APPVIEW);
}

AppView::~AppView()
{
    ILOG_TRACE_W(ILX_APPVIEW);
}

void
AppView::show(AnimatedProperty props, int tx, int ty)
{
    if (_cState != APPCOMP_READY)
        return;

    if (_animProps & AnimShowing)
        return;

    ILOG_TRACE_W(ILX_APPVIEW);
    ILOG_DEBUG(ILX_APPVIEW, " -> %s\n", _instance->appInfo()->name().c_str());
    ILOG_DEBUG(ILX_APPVIEW, " -> geom: %d, %d, %d, %d\n", x(), y(), width(), height());

    _animProps = props;
    bool anim = false;
    _propAnim.stop();
    if (_animProps & Opacity)
    {
        _opacityTween->setEnabled(true);
        _opacityTween->setRange(0, 255);
        setOpacity(0);
        anim = true;
        ILOG_DEBUG(ILX_APPVIEW, " -> Opacity\n");
    } else
    {
        _opacityTween->setEnabled(false);
        setOpacity(255);
    }

    if (_animProps & Zoom)
    {
        _zoomTween->setEnabled(true);
        _zoomTween->setRange(0.8, 1);
        setZoomFactor(0.8);
        anim = true;
        ILOG_DEBUG(ILX_APPVIEW, " -> Zoom\n");
    } else
    {
        _zoomTween->setEnabled(false);
        setZoomFactor(1);
    }

    if (_animProps & Position)
    {
        if (x() != tx)
        {
            _xTween->setEnabled(true);
            _xTween->setRange(x(), tx);
            anim = true;
            ILOG_DEBUG(ILX_APPVIEW, " -> x\n");
        } else
            _xTween->setEnabled(false);

        if (y() != ty)
        {
            _yTween->setEnabled(true);
            _yTween->setRange(y(), ty);
            anim = true;
            ILOG_DEBUG(ILX_APPVIEW, " -> y\n");
        } else
            _yTween->setEnabled(false);
    } else
    {
        _xTween->setEnabled(false);
        _yTween->setEnabled(false);
    }

    setVisible(true);
    clearAnimatedProperty(HideWhenDone);
    clearAnimatedProperty(AnimHiding);
    setAnimatedProperty(AnimShowing);
    setFocus();

    if (anim)
    {
        ILOG_DEBUG(ILX_APPVIEW, " -> props: %x\n", _animProps);
        _propAnim.setDuration(_compositor->settings.durationShow);
        _propAnim.start();
    } else
    {
        _compositor->appVisible();
        update();
    }
}

void
AppView::hide(AnimatedProperty props, int tx, int ty)
{
    if (_cState != APPCOMP_READY)
        return;

    if (_animProps & AnimHiding)
        return;

    ILOG_TRACE_W(ILX_APPVIEW);
    ILOG_DEBUG(ILX_APPVIEW, " -> %s\n", _instance->appInfo()->name().c_str());

    _animProps = props;
    bool anim = false;
    _propAnim.stop();
    if (_animProps & Opacity)
    {
        _opacityTween->setEnabled(true);
        _opacityTween->setRange(255, 0);
        setOpacity(255);
        anim = true;
        ILOG_DEBUG(ILX_APPVIEW, " -> Opacity\n");
    } else
        _opacityTween->setEnabled(false);

    if (_animProps & Zoom)
    {
        _zoomTween->setEnabled(true);
        _zoomTween->setRange(1, 2);
        setZoomFactor(1);
        anim = true;
        ILOG_DEBUG(ILX_APPVIEW, " -> Zoom\n");
    } else
        _zoomTween->setEnabled(false);

    if (_animProps & Position)
    {
        if (x() != tx)
        {
            _xTween->setEnabled(true);
            _xTween->setRange(x(), tx);
            anim = true;
            ILOG_DEBUG(ILX_APPVIEW, " -> x\n");
        } else
            _xTween->setEnabled(false);

        if (y() != ty)
        {
            _yTween->setEnabled(true);
            _yTween->setRange(y(), ty);
            anim = true;
            ILOG_DEBUG(ILX_APPVIEW, " -> y\n");
        } else
            _yTween->setEnabled(false);
    } else
    {
        _xTween->setEnabled(false);
        _yTween->setEnabled(false);
    }

    setAnimatedProperty(HideWhenDone);
    setAnimatedProperty(AnimHiding);
    clearAnimatedProperty(AnimShowing);

    if (anim)
    {
        ILOG_DEBUG(ILX_APPVIEW, " -> props: %x\n", _animProps);
        _propAnim.setDuration(_compositor->settings.durationHide);
        _propAnim.start();
    } else
    {
        setVisible(false);
        update();
    }
}

void
AppView::setAnimatedProperty(AnimatedProperty prop)
{
    _animProps = (AnimatedProperty) (_animProps | prop);
}

void
AppView::clearAnimatedProperty(AnimatedProperty prop)
{
    _animProps = (AnimatedProperty) (_animProps & ~prop);
}

void
AppView::slideTo(int tx, int ty)
{
    ILOG_TRACE_W(ILX_APPVIEW);
    ILOG_DEBUG(ILX_APPVIEW, " -> %s\n", _instance->appInfo()->name().c_str());
    ILOG_DEBUG(ILX_APPVIEW, " -> geom: %d, %d, %d, %d\n", x(), y(), width(), height());

    if ((_cState != APPCOMP_READY) || (_animProps & AnimShowing) || (_animProps & AnimHiding))
        return;

    _propAnim.stop();
    bool anim = false;
    if (x() != tx)
    {
        _xTween->setEnabled(true);
        _xTween->setRange(x(), tx);
        anim = true;
        ILOG_DEBUG(ILX_APPVIEW, " -> x=%d\n", tx);
    } else
        _xTween->setEnabled(false);

    if (y() != ty)
    {
        _yTween->setEnabled(true);
        _yTween->setRange(y(), ty);
        anim = true;
        ILOG_DEBUG(ILX_APPVIEW, " -> y=%d\n", ty);
    } else
        _yTween->setEnabled(false);

    _opacityTween->setEnabled(false);
    _zoomTween->setEnabled(false);

    if (anim)
    {
        ILOG_DEBUG(ILX_APPVIEW, " -> props: %x\n", _animProps);
        _propAnim.setDuration(_compositor->settings.durationSlide);
        _propAnim.start();
    }
}

void
AppView::tweenSlot()
{
    if (_opacityTween->enabled())
        setOpacity(_opacityTween->value());

    if (_zoomTween->enabled())
        setZoomFactor(_zoomTween->value());

    if (_xTween->enabled())
        setX(_xTween->value());

    if (_yTween->enabled())
        setY(_yTween->value());

    update();
}

void
AppView::tweenEndSlot()
{
    ILOG_TRACE_W(ILX_APPVIEW);
    ILOG_DEBUG(ILX_APPVIEW, " -> %s\n", _instance->appInfo()->name().c_str());
    if (_animProps & AnimShowing)
        _compositor->appVisible();

    if (_animProps & HideWhenDone)
        setVisible(false);
    clearAnimatedProperty(AnimShowing);
    clearAnimatedProperty(AnimHiding);
}

void
AppView::madeAvailable()
{
    ILOG_TRACE_W(ILX_APPVIEW);
    ILOG_DEBUG(ILX_APPVIEW, " -> %s\n", _instance->appInfo()->name().c_str());
    if (_cState == APPCOMP_NONE)
    {
        _cState = APPCOMP_READY;
        AppFlags flags = _instance->appInfo()->appFlags();
        if (flags & APP_STATUSBAR)
            show();
        else if (flags & APP_OSK)
            _compositor->toggleOSK(true);
        else if (flags & APP_AUTO_START)
            _compositor->_compComp->notifyVisibility(_instance, false);
        else
            _compositor->showInstance(_instance);
    }
}

} /* namespace ilixi */
