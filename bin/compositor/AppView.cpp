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

#include "AppView.h"
#include <core/Logger.h>
#include "Compositor.h"

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_APPVIEW, "ilixi/comp/AppView", "AppView");

int AppView::_animDuration = 500;

AppView::AppView(Compositor* compositor, AppInstance* instance, Widget* parent)
        : AppCompositor(compositor, instance, parent),
          _animProps((AnimatedProperty) (Opacity))
{
    setInputMethod(PointerInput);

    _propAnim.setDuration(_animDuration);
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

    _animProps = props;
    bool anim = false;
    _propAnim.stop();
    if (_animProps & Opacity)
    {
        _opacityTween->setEnabled(true);
        _opacityTween->setInitialValue(0);
        _opacityTween->setEndValue(255);
        setOpacity(0);
        anim = true;
        ILOG_DEBUG(ILX_APPVIEW, " -> Opacity\n");
    } else
        _opacityTween->setEnabled(false);

    if (_animProps & Zoom)
    {
        _zoomTween->setEnabled(true);
        _zoomTween->setInitialValue(0.8);
        _zoomTween->setEndValue(1);
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
            _xTween->setInitialValue(x());
            _xTween->setEndValue(tx);
            anim = true;
            ILOG_DEBUG(ILX_APPVIEW, " -> x\n");
        }
        if (y() != ty)
        {
            _yTween->setEnabled(true);
            _yTween->setInitialValue(y());
            _yTween->setEndValue(ty);
            anim = true;
            ILOG_DEBUG(ILX_APPVIEW, " -> y\n");
        }
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
        _propAnim.setDuration(300);
        _propAnim.start();
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
        _opacityTween->setInitialValue(255);
        _opacityTween->setEndValue(0);
        setOpacity(255);
        anim = true;
        ILOG_DEBUG(ILX_APPVIEW, " -> Opacity\n");
    } else
        _opacityTween->setEnabled(false);

    if (_animProps & Zoom)
    {
        _zoomTween->setEnabled(true);
        _zoomTween->setInitialValue(1);
        _zoomTween->setEndValue(2);
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
            _xTween->setInitialValue(x());
            _xTween->setEndValue(tx);
            anim = true;
            ILOG_DEBUG(ILX_APPVIEW, " -> x\n");
        }
        if (y() != ty)
        {
            _yTween->setEnabled(true);
            _yTween->setInitialValue(y());
            _yTween->setEndValue(ty);
            anim = true;
            ILOG_DEBUG(ILX_APPVIEW, " -> y\n");
        }
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
        _propAnim.setDuration(500);
        _propAnim.start();
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
    _propAnim.stop();
    bool anim = false;
    if (x() != tx)
    {
        _xTween->setEnabled(true);
        _xTween->setInitialValue(x());
        _xTween->setEndValue(tx);
        anim = true;
        ILOG_DEBUG(ILX_APPVIEW, " -> x=%d\n", tx);
    }

    if (y() != ty)
    {
        _yTween->setEnabled(true);
        _yTween->setInitialValue(y());
        _yTween->setEndValue(ty);
        anim = true;
        ILOG_DEBUG(ILX_APPVIEW, " -> y=%d\n", ty);
    }

    _opacityTween->setEnabled(false);
    _zoomTween->setEnabled(false);

    if (anim) {
        ILOG_DEBUG(ILX_APPVIEW, " -> props: %x\n", _animProps);
        _propAnim.setDuration(300);
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

    if (_xTween->enabled() || _yTween->enabled())
        moveTo(_xTween->value(), _yTween->value());

    update();
}

void
AppView::tweenEndSlot()
{
    if (_animProps & HideWhenDone)
        setVisible(false);
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
