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

#include <ui/Slider.h>
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_SLIDER, "ilixi/ui/Slider", "Slider");

Slider::Slider(Widget* parent)
        : Widget(parent),
          _maximum(100),
          _minimum(0),
          _value(0),
          _step(1),
          _pageStep(10),
          _orientation(Horizontal),
          _inverted(false)
{
    _range = _maximum - _minimum;
    setInputMethod((WidgetInputMethod) (KeyPointerTracking | PointerGrabbing));
    setConstraints(ExpandingConstraint, FixedConstraint);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &Slider::updateIndicatorPosition));
    ILOG_TRACE_W(ILX_SLIDER);
}

Slider::~Slider()
{
    ILOG_TRACE_W(ILX_SLIDER);
}

Size
Slider::preferredSize() const
{
    if (_orientation == Horizontal)
        return stylist()->defaultSize(StyleHint::Slider);
    return stylist()->defaultSize(StyleHint::SliderV);
}

bool
Slider::inverted() const
{
    return _inverted;
}

float
Slider::maximum() const
{
    return _maximum;
}

float
Slider::minimum() const
{
    return _minimum;
}

Orientation
Slider::orientation() const
{
    return _orientation;
}

float
Slider::pageStep() const
{
    return _pageStep;
}

float
Slider::step() const
{
    return _step;
}

float
Slider::range() const
{
    return _range;
}

float
Slider::value() const
{
    return _value;
}

void
Slider::setValue(float value, bool signal)
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
        update();
        if (signal)
            sigValueChanged(_value);
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
Slider::setInverted(bool inverted)
{
    if (_inverted != inverted)
    {
        _inverted = inverted;
        doLayout();
    }
}

void
Slider::setMinimum(float minimum)
{
    if (minimum != _maximum)
    {
        if (minimum >= _maximum)
            _minimum = _maximum;
        else
            _minimum = minimum;

        if (_value < _minimum)
            _value = _minimum;

        _range = _maximum - _minimum;
    }
}

void
Slider::setMaximum(float maximum)
{
    if (maximum != _maximum)
    {
        if (maximum <= _minimum)
            _maximum = _minimum;
        else
            _maximum = maximum;

        if (_value > _maximum)
            _value = _maximum;

        _range = _maximum - _minimum;
    }
}

void
Slider::setOrientation(Orientation orientation)
{
    if (_orientation != orientation)
    {
        _orientation = orientation;
        if (_orientation == Horizontal)
            setConstraints(ExpandingConstraint, FixedConstraint);
        else
            setConstraints(FixedConstraint, ExpandingConstraint);
        doLayout();
    }
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
Slider::keyDownEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.keySymbol)
    {
    case DIKS_CURSOR_LEFT:
    case DIKS_CURSOR_UP:
        if (_orientation == Vertical && !_inverted)
            setValue(_value + _step);
        else
            setValue(_value - _step);

        break;
    case DIKS_CURSOR_RIGHT:
    case DIKS_CURSOR_DOWN:
        if (_orientation == Vertical && !_inverted)
            setValue(_value - _step);
        else
            setValue(_value + _step);

        break;
    case DIKS_PAGE_UP:
        if (_orientation == Vertical && !_inverted)
            setValue(_value + _pageStep);
        else
            setValue(_value - _pageStep);

        break;
    case DIKS_PAGE_DOWN:
        if (_orientation == Vertical && !_inverted)
            setValue(_value - _pageStep);
        else
            setValue(_value + _pageStep);

        break;
    case DIKS_HOME:
        setValue(_minimum);
        break;
    case DIKS_END:
        setValue(_maximum);
        break;
    default:
        break;
    }
}
void
Slider::pointerButtonDownEvent(const PointerEvent& pointerEvent)
{
    setValueUsingPoint(mapToSurface(Point(pointerEvent.x, pointerEvent.y)));
    sigPressed();
}
void
Slider::pointerButtonUpEvent(const PointerEvent& pointerEvent)
{
    update();
    sigReleased();
}

void
Slider::pointerMotionEvent(const PointerEvent& pointerEvent)
{
    if (pressed())
        setValueUsingPoint(mapToSurface(Point(pointerEvent.x, pointerEvent.y)));
}

void
Slider::pointerWheelEvent(const PointerEvent& event)
{
//    setValue(_value - event.wheelStep * _pageStep);
    if (_orientation == Vertical)
    {
        if (_inverted)
            setValue(_value - event.wheelStep * _pageStep);
        else
            setValue(_value + event.wheelStep * _pageStep);
    } else
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
Slider::setValueUsingPoint(const Point& p)
{
    int cursor = 0;
    if (_orientation == Horizontal)
    {

        if (_inverted)
            cursor = width() - p.x() - stylist()->defaultParameter(StyleHint::SliderIndicatorWidth);
        else
            cursor = p.x() - stylist()->defaultParameter(StyleHint::SliderIndicatorWidth);
        setValue((_range * cursor / (width() - stylist()->defaultParameter(StyleHint::SliderIndicatorWidth))) + _minimum);
    } else
    {
        if (_inverted)
            cursor = p.y() - stylist()->defaultParameter(StyleHint::SliderIndicatorHeight);
        else
            cursor = height() - p.y() - stylist()->defaultParameter(StyleHint::SliderIndicatorHeight);
        setValue((_range * cursor / (height() - stylist()->defaultParameter(StyleHint::SliderIndicatorHeight))) + _minimum);
    }
}

void
Slider::updateIndicatorPosition()
{
    ILOG_TRACE_W(ILX_SLIDER);
    float percent;

    if (_orientation == Horizontal)
    {
        if (_inverted)
            percent = (_maximum - _value) / _range;
        else
            percent = (_value - _minimum) / _range;

        _indicator.moveTo((width() - stylist()->defaultParameter(StyleHint::SliderIndicatorWidth)) * percent, 0);
    } else
    {
        if (_inverted)
            percent = (_value - _minimum) / _range;
        else
            percent = (_maximum - _value) / _range;

        _indicator.moveTo(0, (height() - stylist()->defaultParameter(StyleHint::SliderIndicatorHeight)) * percent);
    }
}

} /* namespace ilixi */
