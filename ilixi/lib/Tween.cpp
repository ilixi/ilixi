/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#include "lib/Tween.h"
#include "lib/Easing.h"
#include <stdio.h>

using namespace ilixi;

Tween::Tween(Transition transition, Equation equation, float& variable,
    float initialValue, float endValue) :
  _transition(transition), _equation(equation), _variable(&variable),
      _initialValue(initialValue), _change(endValue - initialValue)
{
}

Tween::Tween(const Tween& t) :
  _transition(t._transition), _equation(t._equation), _variable(t._variable),
      _initialValue(t._initialValue), _change(t._change)
{
}

Tween::~Tween()
{
}

float
Tween::getEndValue() const
{
  return _change + _initialValue;
}

Tween::Equation
Tween::getEquation() const
{
  return _equation;
}

float
Tween::getInitialValue() const
{
  return _initialValue;
}

Tween::Transition
Tween::getTransition() const
{
  return _transition;
}

void
Tween::setEndValue(float endValue)
{
  _change = endValue - _initialValue;
}

void
Tween::setEquation(Equation equation)
{
  _equation = equation;
}

void
Tween::setInitialValue(float initialValue)
{
  float end = getEndValue();
  _initialValue = initialValue;
  _change = end - _initialValue;
}

void
Tween::setTransition(Transition transition)
{
  _transition = transition;
}

void
Tween::runEase(float t, float d)
{
  using namespace Ease;

  switch (_transition)
    {

  case Tween::BACK:
    switch (_equation)
      {
    case Tween::EASE_IN:
      *_variable = Back::easeIn(t, _initialValue, _change, d);
      return;
    case Tween::EASE_OUT:
      *_variable = Back::easeOut(t, _initialValue, _change, d);
      return;
    case Tween::EASE_IN_OUT:
      *_variable = Back::easeInOut(t, _initialValue, _change, d);
      return;
      }

  case Tween::BOUNCE:
    switch (_equation)
      {
    case Tween::EASE_IN:
      *_variable = Bounce::easeIn(t, _initialValue, _change, d);
      return;
    case Tween::EASE_OUT:
      *_variable = Bounce::easeOut(t, _initialValue, _change, d);
      return;
    case Tween::EASE_IN_OUT:
      *_variable = Bounce::easeInOut(t, _initialValue, _change, d);
      return;
      }

  case Tween::CIRCLE:
    switch (_equation)
      {
    case Tween::EASE_IN:
      *_variable = Circle::easeIn(t, _initialValue, _change, d);
      return;
    case Tween::EASE_OUT:
      *_variable = Circle::easeOut(t, _initialValue, _change, d);
      return;
    case Tween::EASE_IN_OUT:
      *_variable = Circle::easeInOut(t, _initialValue, _change, d);
      return;
      }

  case Tween::CUBIC:
    switch (_equation)
      {
    case Tween::EASE_IN:
      *_variable = Cubic::easeIn(t, _initialValue, _change, d);
      return;
    case Tween::EASE_OUT:
      *_variable = Cubic::easeOut(t, _initialValue, _change, d);
      return;
    case Tween::EASE_IN_OUT:
      *_variable = Cubic::easeInOut(t, _initialValue, _change, d);
      return;
      }

  case Tween::ELASTIC:
    switch (_equation)
      {
    case Tween::EASE_IN:
      *_variable = Elastic::easeIn(t, _initialValue, _change, d);
      return;
    case Tween::EASE_OUT:
      *_variable = Elastic::easeOut(t, _initialValue, _change, d);
      return;
    case Tween::EASE_IN_OUT:
      *_variable = Elastic::easeInOut(t, _initialValue, _change, d);
      return;
      }

  case Tween::EXPO:
    switch (_equation)
      {
    case Tween::EASE_IN:
      *_variable = Expo::easeIn(t, _initialValue, _change, d);
      return;
    case Tween::EASE_OUT:
      *_variable = Expo::easeOut(t, _initialValue, _change, d);
      return;
    case Tween::EASE_IN_OUT:
      *_variable = Expo::easeInOut(t, _initialValue, _change, d);
      return;
      }

  case Tween::QUAD:
    switch (_equation)
      {
    case Tween::EASE_IN:
      *_variable = Quad::easeIn(t, _initialValue, _change, d);
      return;
    case Tween::EASE_OUT:
      *_variable = Quad::easeOut(t, _initialValue, _change, d);
      return;
    case Tween::EASE_IN_OUT:
      *_variable = Quad::easeInOut(t, _initialValue, _change, d);
      return;
      }

  case Tween::QUART:
    switch (_equation)
      {
    case Tween::EASE_IN:
      *_variable = Quart::easeIn(t, _initialValue, _change, d);
      return;
    case Tween::EASE_OUT:
      *_variable = Quart::easeOut(t, _initialValue, _change, d);
      return;
    case Tween::EASE_IN_OUT:
      *_variable = Quart::easeInOut(t, _initialValue, _change, d);
      return;
      }

  case Tween::QUINT:
    switch (_equation)
      {
    case Tween::EASE_IN:
      *_variable = Quint::easeIn(t, _initialValue, _change, d);
      return;
    case Tween::EASE_OUT:
      *_variable = Quint::easeOut(t, _initialValue, _change, d);
      return;
    case Tween::EASE_IN_OUT:
      *_variable = Quint::easeInOut(t, _initialValue, _change, d);
      return;
      }

  case Tween::SINE:
    switch (_equation)
      {
    case Tween::EASE_IN:
      *_variable = Sine::easeIn(t, _initialValue, _change, d);
      return;
    case Tween::EASE_OUT:
      *_variable = Sine::easeOut(t, _initialValue, _change, d);
      return;
    case Tween::EASE_IN_OUT:
      *_variable = Sine::easeInOut(t, _initialValue, _change, d);
      return;
      }

  case Tween::LINEAR:
    switch (_equation)
      {
    case Tween::EASE_IN:
      *_variable = Linear::easeIn(t, _initialValue, _change, d);
      return;
    case Tween::EASE_OUT:
      *_variable = Linear::easeOut(t, _initialValue, _change, d);
      return;
    case Tween::EASE_IN_OUT:
      *_variable = Linear::easeInOut(t, _initialValue, _change, d);
      return;
      }

  default:
    return;
    }
}
