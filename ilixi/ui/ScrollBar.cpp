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

#include <ui/ScrollBar.h>
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_SCROLLBAR, "ilixi/ui/ScrollBar", "ScrollBar");

ScrollBar::ScrollBar(Orientation orientation, Widget* parent)
        : Widget(parent),
          _min(0),
          _max(100),
          _value(0),
          _step(1),
          _direction(Down)
{
    setInputMethod((WidgetInputMethod) (PointerTracking | PointerGrabbing));
    setConstraints(ExpandingConstraint, FixedConstraint);

    _dec = new DirectionalButton();
    _dec->sigClicked.connect(sigc::mem_fun(this, &ScrollBar::scrollDec));
    _dec->setDisabled();
    addChild(_dec);

    _inc = new DirectionalButton();
    _inc->sigClicked.connect(sigc::mem_fun(this, &ScrollBar::scrollInc));
    addChild(_inc);

    setOrientation(orientation);

    sigGeometryUpdated.connect(sigc::mem_fun(this, &ScrollBar::updateSBGeometry));
}

ScrollBar::~ScrollBar()
{
}

Size
ScrollBar::preferredSize() const
{
    ILOG_TRACE_W(ILX_SCROLLBAR);
    Size s = _dec->preferredSize();
    if (_orientation == Horizontal)
        return Size(2 * s.width() + 50, s.height());
    return Size(s.width(), 2 * s.height() + 50);
}

const Rectangle&
ScrollBar::indicator() const
{
    return _indicator;
}

const Rectangle&
ScrollBar::indicatorRegion() const
{
    return _indicatorRegion;
}

Direction
ScrollBar::direction() const
{
    return _direction;
}

int
ScrollBar::max() const
{
    return _max;
}

void
ScrollBar::setMax(int max)
{
    _max = max;
}

int
ScrollBar::min() const
{
    return _min;
}

void
ScrollBar::setMin(int min)
{
    _min = min;
}

Orientation
ScrollBar::orientation() const
{
    return _orientation;
}

void
ScrollBar::setOrientation(Orientation orientation)
{
    _orientation = orientation;
    if (_orientation == Horizontal)
    {
        setConstraints(ExpandingConstraint, FixedConstraint);
        _dec->setIcon(StyleHint::Left);
        _inc->setIcon(StyleHint::Right);

        if (_direction == Up)
        {
            _dec->setCorners((Corners) TopLeft);
            _inc->setCorners((Corners) TopRight);
        } else
        {
            _dec->setCorners((Corners) BottomLeft);
            _inc->setCorners((Corners) BottomRight);
        }

    } else
    {
        setConstraints(FixedConstraint, ExpandingConstraint);
        _dec->setIcon(StyleHint::Up);
        _inc->setIcon(StyleHint::Down);

        if (_direction == Left)
        {
            _dec->setCorners((Corners) TopLeft);
            _inc->setCorners((Corners) BottomLeft);
        } else
        {
            _dec->setCorners((Corners) TopRight);
            _inc->setCorners((Corners) BottomRight);
        }

    }
    doLayout();
}

void
ScrollBar::setRange(int min, int max)
{
    _min = min;
    _max = max;
    if (_value < _min)
        _value = _min;
    else if (_value > _max)
        _value = _max;
}

int
ScrollBar::step() const
{
    return _step;
}

void
ScrollBar::setStep(int step)
{
    _step = step;
}

int
ScrollBar::value() const
{
    return _value;
}

void
ScrollBar::setDirection(Direction direction)
{
    _direction = direction;
}

void
ScrollBar::setValue(int value)
{
    if (_value != value)
    {
        if (value <= _min)
        {
            _value = _min;
            _dec->setDisabled();
            _inc->setEnabled();
        } else if (value >= _max)
        {
            _value = _max;
            _dec->setEnabled();
            _inc->setDisabled();
        } else
        {
            _value = value;
            _dec->setEnabled();
            _inc->setEnabled();
        }
        updateIndicatorGeometry();
        update();
        sigValueChanged(_value);
    }
}

void
ScrollBar::scroll(int amount)
{
    setValue(_value + amount);
}

void
ScrollBar::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    stylist()->drawScrollBar(&p, this);
}

void
ScrollBar::pointerButtonDownEvent(const PointerEvent& pointerEvent)
{
    setSBIndicator(mapToSurface(Point(pointerEvent.x, pointerEvent.y)));
    sigPressed();
}
void
ScrollBar::pointerButtonUpEvent(const PointerEvent& pointerEvent)
{
    sigReleased();
    update();
}

void
ScrollBar::pointerMotionEvent(const PointerEvent& pointerEvent)
{
    if (pressed())
        setSBIndicator(mapToSurface(Point(pointerEvent.x, pointerEvent.y)));
}

void
ScrollBar::pointerWheelEvent(const PointerEvent& event)
{
    if (_orientation == Vertical)
        setValue(_value - event.wheelStep * _step);
    else
        setValue(_value - event.wheelStep * _step);
}

void
ScrollBar::setSBIndicator(const Point& p)
{
    int cursor = 0;
    if (_orientation == Horizontal)
    {
        if (p.x() < _pRect.x())
            cursor = 0;
        else if (p.x() > _pRect.right())
            cursor = _pRect.width();
        else
            cursor = p.x() - _pRect.x();
        setValue(((_max - _min) * cursor / (_indicatorRegion.width() - _indicator.width() - .0)) + _min);
    } else
    {
        if (p.y() < _pRect.y())
            cursor = 0;
        else if (p.y() > _pRect.bottom())
            cursor = _pRect.height();
        else
            cursor = p.y() - _pRect.y();
        setValue(((_max - _min) * cursor / (_indicatorRegion.height() - _indicator.height() - .0)) + _min);
    }
}

void
ScrollBar::updateIndicatorGeometry()
{
    if (_orientation == Horizontal)
        _indicator.setX(_indicatorRegion.x() + (_indicatorRegion.width() - _indicator.width() - .0) * _value / (_max - _min));
    else
        _indicator.setY(_indicatorRegion.y() + (_indicatorRegion.height() - _indicator.height() - .0) * _value / (_max - _min));
}

void
ScrollBar::scrollDec()
{
    scroll(-_step);
}

void
ScrollBar::scrollInc()
{
    scroll(_step);
}

void
ScrollBar::updateSBGeometry()
{
    ILOG_TRACE_W(ILX_SCROLLBAR);
    Size s = _dec->preferredSize();
    _dec->setNeighbours(getNeighbour(Up), getNeighbour(Down), getNeighbour(Left), getNeighbour(Right));
    _inc->setNeighbours(getNeighbour(Up), getNeighbour(Down), getNeighbour(Left), getNeighbour(Right));

    if (_orientation == Horizontal)
    {
        _dec->setGeometry(0, 0, s.width(), height());
        _inc->setGeometry(width() - s.width(), 0, s.width(), height());

        _dec->setNeighbour(Right, _inc);
        _inc->setNeighbour(Right, _dec);

        int wUsed = _dec->width() + _inc->width();
        _indicatorRegion.setRectangle(_dec->width(), 0, width() - wUsed, height());
        int w = _step * (_indicatorRegion.width() - wUsed - .0) / (_max - _min);
        if (w < stylist()->defaultParameter(StyleHint::ToolButtonLR))
            w = stylist()->defaultParameter(StyleHint::ToolButtonLR);
        _indicator.setSize(w, height());
        _indicator.moveTo(_indicatorRegion.x() + (_indicatorRegion.width() - _indicator.width()) * _value / (_max - _min), 0);

        _pRect.moveTo(_indicatorRegion.x() + _indicator.width() / 2, _indicatorRegion.y());
        _pRect.setSize(_indicatorRegion.width() - _indicator.width(), _indicatorRegion.height());
    } else
    {
        _dec->setGeometry(0, 0, width(), s.height());
        _inc->setGeometry(0, height() - s.height(), width(), s.height());

        _dec->setNeighbour(Down, _inc);
        _inc->setNeighbour(Up, _dec);

        int hUsed = _dec->height() + _inc->height();
        _indicatorRegion.setRectangle(0, _dec->height(), width(), height() - hUsed);
        int h = _step * (_indicatorRegion.height() - hUsed - .0) / (_max - _min);
        if (h < stylist()->defaultParameter(StyleHint::ToolButtonTB))
            h = stylist()->defaultParameter(StyleHint::ToolButtonTB);
        _indicator.setSize(width(), h);
        _indicator.moveTo(0, _indicatorRegion.y() + (_indicatorRegion.height() - _indicator.height()) * _value / (_max - _min));

        _pRect.moveTo(_indicatorRegion.x(), _indicatorRegion.y() + _indicator.height() / 2);
        _pRect.setSize(_indicatorRegion.width(), _indicatorRegion.height() - _indicator.height());
    }
}

} /* namespace ilixi */
