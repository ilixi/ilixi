/*
 * HomeButton.cpp
 *
 *  Created on: Apr 3, 2012
 *      Author: tarik
 */

#include "HomeButton.h"
#include "graphics/Painter.h"
#include "core/Logger.h"

namespace ilixi
{

  HomeButton::HomeButton(Widget* parent) :
      Widget(parent), _click(false), _aniVal(1)
  {
    setInputMethod(PointerInput);
    setConstraints(FixedConstraint, FixedConstraint);

    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &HomeButton::updateHBGeometry));
    _image = new Image(ILIXI_DATADIR"compositor/comp_home.png", 80, 80);
    _anim.setDuration(500);
    _tween = new Tween(Tween::SINE, Tween::EASE_OUT, _aniVal, 0, 1);
    _anim.addTween(_tween);

    _anim.sigExec.connect(sigc::mem_fun(this, &HomeButton::tweenSlot));

    _anim.sigFinished.connect(sigc::mem_fun(this, &HomeButton::tweenEndSlot));
    setVisible(false);
  }

  HomeButton::~HomeButton()
  {
    delete _image;
  }

  Size
  HomeButton::preferredSize() const
  {
    return Size(80, 80);
  }

  void
  HomeButton::show()
  {
    _anim.stop();
    setVisible(true);
    _tween->setInitialValue(1);
    _tween->setEndValue(0);
    _anim.start();
  }

  void
  HomeButton::hide()
  {
    _anim.stop();
    _tween->setInitialValue(0);
    _tween->setEndValue(1);
    _anim.start();
  }

  void
  HomeButton::pointerButtonDownEvent(const PointerEvent& pointerEvent)
  {
    _click = true;
  }

  void
  HomeButton::pointerButtonUpEvent(const PointerEvent& pointerEvent)
  {
    if (_click)
      {
        sigPressed();
        _click = false;
      }
  }

  void
  HomeButton::compose()
  {
    Painter p(this);
    p.begin();
    p.setBrush(Color(0, 0, 0, 255));
    p.drawImage(_image, -80 * _aniVal, -80 * _aniVal);
    p.end();
  }

  void
  HomeButton::tweenSlot()
  {
    update();
  }

  void
  HomeButton::tweenEndSlot()
  {
    if (_aniVal == 1)
      setVisible(false);
  }

  void
  HomeButton::updateHBGeometry()
  {
  }

} /* namespace ilixi */
