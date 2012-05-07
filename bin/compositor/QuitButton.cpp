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

#include "QuitButton.h"
#include "graphics/Painter.h"
#include "core/Logger.h"

namespace ilixi
{

  QuitButton::QuitButton(Widget* parent) :
      Widget(parent), _click(false)
  {
    setInputMethod(PointerInput);
    setConstraints(FixedConstraint, FixedConstraint);

    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &QuitButton::updateHBGeometry));
    _image = new Image(ILIXI_DATADIR"compositor/comp_quit.png", 80, 80);
    _anim.setDuration(500);
    _tween = new Tween(Tween::SINE, Tween::EASE_OUT, 0, 1);
    _anim.addTween(_tween);

    _anim.sigExec.connect(sigc::mem_fun(this, &QuitButton::tweenSlot));

    _anim.sigFinished.connect(sigc::mem_fun(this, &QuitButton::tweenEndSlot));
    setVisible(false);
  }

  QuitButton::~QuitButton()
  {
    delete _image;
  }

  Size
  QuitButton::preferredSize() const
  {
    return Size(80, 80);
  }

  void
  QuitButton::show()
  {
    _anim.stop();
    setVisible(true);
    _tween->setInitialValue(1);
    _tween->setEndValue(0);
    _anim.start();
  }

  void
  QuitButton::hide()
  {
    _anim.stop();
    _tween->setInitialValue(0);
    _tween->setEndValue(1);
    _anim.start();
  }

  void
  QuitButton::pointerButtonDownEvent(const PointerEvent& pointerEvent)
  {
    _click = true;
  }

  void
  QuitButton::pointerButtonUpEvent(const PointerEvent& pointerEvent)
  {
    if (_click)
      {
        sigPressed();
        _click = false;
      }
  }

  void
  QuitButton::compose(const Rectangle& rect)
  {
    Painter p(this);
    p.begin(rect);
    p.setBrush(Color(0, 0, 0, 255));
    p.drawImage(_image, 80 * _tween->value(), -80 * _tween->value());
    p.end();
  }

  void
  QuitButton::tweenSlot()
  {
    update();
  }

  void
  QuitButton::tweenEndSlot()
  {
    if (_tween->value() == 1)
      setVisible(false);
  }

  void
  QuitButton::updateHBGeometry()
  {
  }

} /* namespace ilixi */
