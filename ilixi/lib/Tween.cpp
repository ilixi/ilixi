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

#include <lib/Tween.h>
#include <lib/Easing.h>
#include <stdio.h>

namespace ilixi
{

Tween::Tween(Transition transition, Equation equation, float initialValue, float endValue)
        : _enabled(true),
          _transition(transition),
          _equation(equation),
          _initialValue(initialValue),
          _change(endValue - initialValue),
          _value(initialValue)
{
}

Tween::Tween(const Tween& t)
        : _enabled(true),
          _transition(t._transition),
          _equation(t._equation),
          _initialValue(t._initialValue),
          _change(t._change),
          _value(t._initialValue)
{
}

Tween::~Tween()
{
}

Tween::Transition
Tween::transition() const
{
    return _transition;
}

Tween::Equation
Tween::equation() const
{
    return _equation;
}

float
Tween::initialValue() const
{
    return _initialValue;
}

float
Tween::endValue() const
{
    return _change + _initialValue;
}

void
Tween::setTransition(Transition transition)
{
    _transition = transition;
}

void
Tween::setEquation(Equation equation)
{
    _equation = equation;
}

void
Tween::setInitialValue(float initialValue)
{
    float end = endValue();
    _initialValue = initialValue;
    _change = end - _initialValue;
    _value = _initialValue;
}

void
Tween::setEndValue(float endValue)
{
    _change = endValue - _initialValue;
}

void
Tween::setRange(float initial, float end)
{
    _initialValue = initial;
    _change = end - _initialValue;
    _value = _initialValue;
}

bool
Tween::enabled() const
{
    return _enabled;
}

void
Tween::setEnabled(bool enabled)
{
    _enabled = enabled;
}

void
Tween::runEase(float t, float d)
{
    using namespace Ease;

    if (!_enabled)
        return;

    switch (_transition)
    {

    case Tween::BACK:
        switch (_equation)
        {
        case Tween::EASE_IN:
            _value = Back::easeIn(t, _initialValue, _change, d);
            return;
        case Tween::EASE_OUT:
            _value = Back::easeOut(t, _initialValue, _change, d);
            return;
        case Tween::EASE_IN_OUT:
            _value = Back::easeInOut(t, _initialValue, _change, d);
            return;
        }
        break;

    case Tween::BOUNCE:
        switch (_equation)
        {
        case Tween::EASE_IN:
            _value = Bounce::easeIn(t, _initialValue, _change, d);
            return;
        case Tween::EASE_OUT:
            _value = Bounce::easeOut(t, _initialValue, _change, d);
            return;
        case Tween::EASE_IN_OUT:
            _value = Bounce::easeInOut(t, _initialValue, _change, d);
            return;
        }
        break;

    case Tween::CIRCLE:
        switch (_equation)
        {
        case Tween::EASE_IN:
            _value = Circle::easeIn(t, _initialValue, _change, d);
            return;
        case Tween::EASE_OUT:
            _value = Circle::easeOut(t, _initialValue, _change, d);
            return;
        case Tween::EASE_IN_OUT:
            _value = Circle::easeInOut(t, _initialValue, _change, d);
            return;
        }
        break;

    case Tween::CUBIC:
        switch (_equation)
        {
        case Tween::EASE_IN:
            _value = Cubic::easeIn(t, _initialValue, _change, d);
            return;
        case Tween::EASE_OUT:
            _value = Cubic::easeOut(t, _initialValue, _change, d);
            return;
        case Tween::EASE_IN_OUT:
            _value = Cubic::easeInOut(t, _initialValue, _change, d);
            return;
        }
        break;

    case Tween::ELASTIC:
        switch (_equation)
        {
        case Tween::EASE_IN:
            _value = Elastic::easeIn(t, _initialValue, _change, d);
            return;
        case Tween::EASE_OUT:
            _value = Elastic::easeOut(t, _initialValue, _change, d);
            return;
        case Tween::EASE_IN_OUT:
            _value = Elastic::easeInOut(t, _initialValue, _change, d);
            return;
        }
        break;

    case Tween::EXPO:
        switch (_equation)
        {
        case Tween::EASE_IN:
            _value = Expo::easeIn(t, _initialValue, _change, d);
            return;
        case Tween::EASE_OUT:
            _value = Expo::easeOut(t, _initialValue, _change, d);
            return;
        case Tween::EASE_IN_OUT:
            _value = Expo::easeInOut(t, _initialValue, _change, d);
            return;
        }
        break;

    case Tween::QUAD:
        switch (_equation)
        {
        case Tween::EASE_IN:
            _value = Quad::easeIn(t, _initialValue, _change, d);
            return;
        case Tween::EASE_OUT:
            _value = Quad::easeOut(t, _initialValue, _change, d);
            return;
        case Tween::EASE_IN_OUT:
            _value = Quad::easeInOut(t, _initialValue, _change, d);
            return;
        }
        break;

    case Tween::QUART:
        switch (_equation)
        {
        case Tween::EASE_IN:
            _value = Quart::easeIn(t, _initialValue, _change, d);
            return;
        case Tween::EASE_OUT:
            _value = Quart::easeOut(t, _initialValue, _change, d);
            return;
        case Tween::EASE_IN_OUT:
            _value = Quart::easeInOut(t, _initialValue, _change, d);
            return;
        }
        break;

    case Tween::QUINT:
        switch (_equation)
        {
        case Tween::EASE_IN:
            _value = Quint::easeIn(t, _initialValue, _change, d);
            return;
        case Tween::EASE_OUT:
            _value = Quint::easeOut(t, _initialValue, _change, d);
            return;
        case Tween::EASE_IN_OUT:
            _value = Quint::easeInOut(t, _initialValue, _change, d);
            return;
        }
        break;

    case Tween::SINE:
        switch (_equation)
        {
        case Tween::EASE_IN:
            _value = Sine::easeIn(t, _initialValue, _change, d);
            return;
        case Tween::EASE_OUT:
            _value = Sine::easeOut(t, _initialValue, _change, d);
            return;
        case Tween::EASE_IN_OUT:
            _value = Sine::easeInOut(t, _initialValue, _change, d);
            return;
        }
        break;

    case Tween::LINEAR:
        switch (_equation)
        {
        case Tween::EASE_IN:
            _value = Linear::easeIn(t, _initialValue, _change, d);
            return;
        case Tween::EASE_OUT:
            _value = Linear::easeOut(t, _initialValue, _change, d);
            return;
        case Tween::EASE_IN_OUT:
            _value = Linear::easeInOut(t, _initialValue, _change, d);
            return;
        }
        break;

    default:
        return;
    }
}

} /* namespace ilixi */
