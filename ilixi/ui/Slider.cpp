/*
 Copyright 2011 Tarik Sekmen.

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

#include "ui/Slider.h"
#include "graphics/Painter.h"
#include "core/Logger.h"

namespace ilixi
{

  Slider::Slider(Widget* parent) :
      Widget(parent), _maximum(100), _minimum(0), _value(0), _step(1), _pageStep(
          10), _orientation(Horizontal), _inverted(false)
  {
    _range = _maximum - _minimum;
    setInputMethod(KeyAndPointerInputTracking);
    setConstraints(ExpandingConstraint, FixedConstraint);
    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &Slider::updateSliderGeometry));
    ILOG_TRACE_W(ILX_SLIDER);
  }

  Slider::~Slider()
  {
    ILOG_TRACE_W(ILX_SLIDER);
  }

  Size
  Slider::preferredSize() const
  {
    Size s = stylist()->defaultSize(StyleHint::Slider);
    if (_orientation == Horizontal)
      return s;
    s.transpose();
    return s;
  }

  float
  Slider::value() const
  {
    return _value;
  }

  void
  Slider::setValue(float value)
  {
    if (value != _value)
      {
        if (value < _minimum)
          _value = _minimum;
        else if (value > _maximum)
          _value = _maximum;
        else
          _value = value;

        updateIndicatorPosition();
        sigValueChanged(_value);
        update();
        ILOG_DEBUG(ILX_SLIDER, "Value: %f\n", _value);
      }
  }

  void
  Slider::setRange(float minimum, float maximum)
  {
    if (minimum >= maximum)
      _minimum = maximum;
    else
      _minimum = minimum;

    if (maximum <= minimum)
      _maximum = minimum;
    else
      _maximum = maximum;

    if (_value < _minimum)
      _value = _minimum;
    else if (_value > _maximum)
      _value = _maximum;

    _range = _maximum - _minimum;
  }

  void
  Slider::setStep(float step)
  {
    _step = step;
  }

  void
  Slider::setPageStep(float pageStep)
  {
    _pageStep = pageStep;
  }

  void
  Slider::compose(const PaintEvent& event)
  {
    ILOG_TRACE_W(ILX_SLIDER);
    Painter p(this);
    p.begin(event);
    stylist()->drawSlider(&p, this);
    p.end();
  }

  void
  Slider::keyUpEvent(const KeyEvent& keyEvent)
  {
    switch (keyEvent.keySymbol)
      {
    case DIKS_CURSOR_LEFT:
    case DIKS_CURSOR_UP:
      if (_orientation == Vertical && !_inverted)
        _value += _step;
      else
        _value -= _step;
      break;
    case DIKS_CURSOR_RIGHT:
    case DIKS_CURSOR_DOWN:
      if (_orientation == Vertical && !_inverted)
        _value -= _step;
      else
        _value += _step;
      break;
    case DIKS_PAGE_UP:
      if (_orientation == Vertical && !_inverted)
        _value += _pageStep;
      else
        _value -= _pageStep;
      break;
    case DIKS_PAGE_DOWN:
      if (_orientation == Vertical && !_inverted)
        _value -= _pageStep;
      else
        _value += _pageStep;
      break;
    case DIKS_HOME:
      _value = _minimum;
      break;
    case DIKS_END:
      _value = _maximum;
      break;
    default:
      break;
      }

    if (_value < _minimum)
      _value = _minimum;
    else if (_value > _maximum)
      _value = _maximum;

    updateIndicatorPosition();
    update();
  }

  void
  Slider::pointerButtonDownEvent(const PointerEvent& pointerEvent)
  {
    Point mouseLocal = mapToSurface(Point(pointerEvent.x, pointerEvent.y));
    if (mouseLocal.x() >= 0 && mouseLocal.x() <= width() - height())
      setValue(_range * (mouseLocal.x() / (width() - height())));
    sigPressed();
  }

  void
  Slider::pointerButtonUpEvent(const PointerEvent& pointerEvent)
  {
    sigReleased();
    update();
  }

  void
  Slider::pointerMotionEvent(const PointerEvent& pointerEvent)
  {
    if (pressed())
      {
        Point mouseLocal = mapToSurface(Point(pointerEvent.x, pointerEvent.y));
        if (mouseLocal.x() >= 0 && mouseLocal.x() <= width() - height())
          setValue(_range * mouseLocal.x() / (width() - height()));
      }
  }
  void
  Slider::pointerWheelEvent(const PointerEvent& event)
  {
    setValue(_value - event.wheelStep * _pageStep);
  }

  void
  Slider::enterEvent(const PointerEvent& event)
  {
    update();
  }

  void
  Slider::leaveEvent(const PointerEvent& event)
  {
    update();
  }

  void
  Slider::focusInEvent()
  {
    update();
  }

  void
  Slider::focusOutEvent()
  {
    update();
  }

  void
  Slider::updateIndicatorPosition()
  {
    ILOG_TRACE_W(ILX_SLIDER);
    _indicator.moveTo((width() - height()) * (_value - _minimum) / _range, 0);
    _indicator.setSize(height(), height());
  }

  void
  Slider::updateSliderGeometry()
  {
    updateIndicatorPosition();
  }

} /* namespace ilixi */
