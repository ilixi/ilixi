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

#include "AppView.h"
#include "core/Logger.h"

namespace ilixi
{

  AppView::AppView(Widget* parent, AppFlags flags) :
      AppCompositor(parent, flags)
  {
    setInputMethod(PointerInputTracking);

    _ani.setDuration(300);
    _ani.sigExec.connect(sigc::mem_fun(this, &AppView::tweenSlot));
    _ani.sigFinished.connect(sigc::mem_fun(this, &AppView::tweenEndSlot));

    _opacityTween = new Tween(Tween::SINE, Tween::EASE_OUT, 0, 255);
    _ani.addTween(_opacityTween);

    _zoomTween = new Tween(Tween::SINE, Tween::EASE_IN, 0.8, 1);
    _ani.addTween(_zoomTween);

    setVisible(false);
  }

  AppView::~AppView()
  {
  }

  void
  AppView::show()
  {
    _ani.stop();
    setVisible(true);
    _opacityTween->setInitialValue(0);
    _opacityTween->setEndValue(255);
    _zoomTween->setInitialValue(0.8);
    _zoomTween->setEndValue(1);
    _ani.start();
  }

  void
  AppView::hide()
  {
    _ani.stop();
    _opacityTween->setInitialValue(255);
    _opacityTween->setEndValue(0);
    _zoomTween->setInitialValue(1);
    _zoomTween->setEndValue(2);
    _ani.start();
  }

  void
  AppView::compose(const Rectangle& rect)
  {
  }

  void
  AppView::tweenSlot()
  {
    setOpacity(_opacityTween->value());
    setZoomFactor(_zoomTween->value());
//    _appCompositor->setGeometry(_scaleTween->value(), _scaleTween->value(),
//        width() - 2 * _scaleTween->value(),
//        height() - 2 * _scaleTween->value());
    update();
  }

  void
  AppView::tweenEndSlot()
  {
    if (_opacityTween->value() == 0)
      setVisible(false);
  }

} /* namespace ilixi */
