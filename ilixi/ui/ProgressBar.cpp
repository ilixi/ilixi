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

#include <ui/ProgressBar.h>
#include <graphics/Painter.h>

namespace ilixi
{

ProgressBar::ProgressBar(Widget* parent)
        : Widget(parent),
          _maximum(100),
          _minimum(0),
          _value(0),
          _orientation(Horizontal)
{
    _range = _maximum - _minimum;
    setConstraints(ExpandingConstraint, FixedConstraint);
}

ProgressBar::~ProgressBar()
{
}

Size
ProgressBar::preferredSize() const
{
    Size s = stylist()->defaultSize(StyleHint::ProgressBar);
    if (orientation() == Horizontal)
        return s;
    s.transpose();
    return s;
}

Orientation
ProgressBar::orientation() const
{
    return _orientation;
}

int
ProgressBar::maximum() const
{
    return _maximum;
}

int
ProgressBar::minimum() const
{
    return _minimum;
}

int
ProgressBar::percentage() const
{
    return _value * 100 / _range;
}

int
ProgressBar::range() const
{
    return _range;
}

int
ProgressBar::value() const
{
    return _value;
}

void
ProgressBar::reset()
{
    _value = 0;
    sigValueChanged(0);
    update();
}

void
ProgressBar::setOrientation(Orientation orientation)
{
    if (_orientation != orientation)
    {
        _orientation = orientation;
        if (_orientation == Horizontal)
            setConstraints(ExpandingConstraint, FixedConstraint);
        else
            setConstraints(FixedConstraint, ExpandingConstraint);
        surface()->setSurfaceFlag(Surface::ModifiedSize);
    }
}

void
ProgressBar::setMaximum(int maximum)
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
ProgressBar::setMinimum(int minimum)
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
ProgressBar::setRange(int minimum, int maximum)
{
    if (minimum >= maximum)
        _minimum = maximum;

    if (maximum <= minimum)
        _maximum = minimum;

    if (_value < _minimum)
        _value = _minimum;
    else if (_value > _maximum)
        _value = _maximum;

    _range = _maximum - _minimum;
}

void
ProgressBar::setValue(int value)
{
    if (value != _value)
    {
        if (value < _minimum)
            _value = _minimum;
        else if (value > _maximum)
            _value = _maximum;
        else
            _value = value;

        sigValueChanged(_value);
        update();
    }
}

void
ProgressBar::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    stylist()->drawProgressBar(&p, this);
    p.end();
}

}
