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

#include "CompositorButton.h"

namespace ilixi
{

  CompositorButton::CompositorButton(const std::string& imagePath,
      Widget* parent) :
      Widget(parent), _click(false)
  {
    setInputMethod(PointerInput);
    setConstraints(FixedConstraint, FixedConstraint);

    _image = new Image(
        std::string(ILIXI_DATADIR"compositor/").append(imagePath), 80, 80);

    _anim.setDuration(500);
    _tween = new Tween(Tween::SINE, Tween::EASE_OUT, 1, 0);
    _anim.addTween(_tween);
    _anim.sigExec.connect(sigc::mem_fun(this, &CompositorButton::tweenSlot));
    _anim.sigFinished.connect(
        sigc::mem_fun(this, &CompositorButton::tweenEndSlot));

    setVisible(false);
  }

  CompositorButton::~CompositorButton()
  {
    delete _image;
  }

  Size
  CompositorButton::preferredSize() const
  {
    return Size(80, 80);
  }

  void
  CompositorButton::show()
  {
    if (!visible())
      {
        _anim.stop();
        _tween->setInitialValue(1);
        _tween->setEndValue(0);
        _anim.start();
        setVisible(true);
      }
  }

  void
  CompositorButton::hide()
  {
    if (visible())
      {
        _anim.stop();
        _tween->setInitialValue(0);
        _tween->setEndValue(1);
        _anim.start();
      }
  }

  void
  CompositorButton::pointerButtonDownEvent(const PointerEvent& pointerEvent)
  {
    _click = true;
  }

  void
  CompositorButton::pointerButtonUpEvent(const PointerEvent& pointerEvent)
  {
    if (_click)
      {
        sigPressed();
        _click = false;
      }
  }

  void
  CompositorButton::tweenSlot()
  {
    update();
  }

  void
  CompositorButton::tweenEndSlot()
  {
    if (_tween->value() == 1)
      setVisible(false);
  }

} /* namespace ilixi */
