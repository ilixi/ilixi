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

#include "AppThumbnail.h"
#include "graphics/Painter.h"

namespace ilixi
{

  AppThumbnail::AppThumbnail(const std::string& text, AppFlags flags,
      Widget* parent) :
      AppCompositor(parent, flags)
  {
    setInputMethod(KeyAndPointerInput);
    setConstraints(FixedConstraint, FixedConstraint);
    _ani.setDuration(500);
    _opacityTween = new Tween(Tween::SINE, Tween::EASE_OUT, 128, 255);
    _overlayTween = new Tween(Tween::SINE, Tween::EASE_OUT, 0, 1);
    _ani.addTween(_opacityTween);
    _ani.addTween(_overlayTween);
    _ani.sigExec.connect(sigc::mem_fun(this, &AppThumbnail::tweenSlot));

    _label = new Label(text);
    addChild(_label);

    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &AppThumbnail::onAppThumbnailGeometryUpdated));

    setVisible(false);
  }

  AppThumbnail::~AppThumbnail()
  {
  }

  Size
  AppThumbnail::preferredSize() const
  {
    return Size(196, 196);
  }

  void
  AppThumbnail::showOverlay()
  {
    // TODO implement.
  }

  void
  AppThumbnail::hideOverlay()
  {
    // TODO implement.
  }

  void
  AppThumbnail::compose()
  {
  }

  void
  AppThumbnail::pointerButtonUpEvent(const PointerEvent& pointerEvent)
  {
//    sigFocused(this);
    sigSelected();
  }

  void
  AppThumbnail::keyUpEvent(const KeyEvent& keyEvent)
  {
//    if (keyEvent.keySymbol == DIKS_SPACE)
//      sigSelected();
  }

  void
  AppThumbnail::focusInEvent()
  {
    sigFocused(this);
    _ani.stop();
    _overlayTween->setInitialValue(0);
    _overlayTween->setEndValue(1);
    _opacityTween->setInitialValue(128);
    _opacityTween->setEndValue(255);
    _ani.start();
  }

  void
  AppThumbnail::focusOutEvent()
  {
    hideOverlay();
    _ani.stop();
    _overlayTween->setInitialValue(1);
    _overlayTween->setEndValue(0);
    _opacityTween->setInitialValue(255);
    _opacityTween->setEndValue(128);
    _ani.start();
  }

  void
  AppThumbnail::tweenSlot()
  {
    setOpacity(_opacityTween->value());
    _label->setY(height() - (height() / 2) * _overlayTween->value());
    update();
  }

  void
  AppThumbnail::onAppThumbnailGeometryUpdated()
  {
    raiseChildToFront(_label);
    _label->setGeometry(0, height(), width(), height() / 2);
  }

} /* namespace ilixi */
