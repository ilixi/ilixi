/*
 * SwitchButton.cpp
 *
 *  Created on: Apr 3, 2012
 *      Author: tarik
 */

#include "SwitchButton.h"
#include "graphics/Painter.h"
#include "core/Logger.h"

namespace ilixi
{

  SwitchButton::SwitchButton(Widget* parent) :
      Widget(parent), _click(false)
  {
    setInputMethod(PointerInput);
    setConstraints(FixedConstraint, FixedConstraint);

    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &SwitchButton::updateSBGeometry));
    _image = new Image(ILIXI_DATADIR"compositor/comp_switch.png", 80, 80);
    _anim.setDuration(500);
    _tween = new Tween(Tween::SINE, Tween::EASE_OUT, 0, 1);
    _anim.addTween(_tween);

    _anim.sigExec.connect(sigc::mem_fun(this, &SwitchButton::tweenSlot));

    _anim.sigFinished.connect(sigc::mem_fun(this, &SwitchButton::tweenEndSlot));
    setVisible(false);
  }

  SwitchButton::~SwitchButton()
  {
    delete _image;
  }

  Size
  SwitchButton::preferredSize() const
  {
    return Size(80, 80);
  }

  void
  SwitchButton::show()
  {
    if (!visible())
      {
        _anim.stop();
        setVisible(true);
        _tween->setInitialValue(1);
        _tween->setEndValue(0);
        _anim.start();
      }
  }

  void
  SwitchButton::hide()
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
  SwitchButton::pointerButtonDownEvent(const PointerEvent& pointerEvent)
  {
    _click = true;
  }

  void
  SwitchButton::pointerButtonUpEvent(const PointerEvent& pointerEvent)
  {
    if (_click)
      {
        sigPressed();
        _click = false;
      }
  }

  void
  SwitchButton::compose(const PaintEvent& event)
  {
    Painter p(this);
    p.begin(event);
    p.setBrush(Color(0, 0, 0, 255));
    p.drawImage(_image, -80 * _tween->value(), 80 * _tween->value());
    p.end();
  }

  void
  SwitchButton::tweenSlot()
  {
    update();
  }

  void
  SwitchButton::tweenEndSlot()
  {
    if (_tween->value() == 1)
      setVisible(false);
  }

  void
  SwitchButton::updateSBGeometry()
  {
  }

} /* namespace ilixi */
