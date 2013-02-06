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

#include <ui/SpinBox.h>
#include <graphics/Painter.h>
#include <core/Logger.h>
#include <sstream>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_SPINBOX, "ilixi/ui/SpinBox", "SpinBox");

SpinBox::SpinBox(int value, Widget* parent)
        : Widget(parent),
          _plus(NULL),
          _minus(NULL),
          _prefix(""),
          _postfix(""),
          _min(0),
          _max(value > 100 ? value : 100),
          _value(value),
          _step(1),
          _wrapping(true)
{
    ILOG_TRACE_W(ILX_SPINBOX);
    setConstraints(FixedConstraint, FixedConstraint);
    setInputMethod(PointerPassthrough);

    _minus = new ToolButton("-");
    _minus->setRepeatable(true);
    _minus->setToolButtonStyle(ToolButton::IconOnly);
    _minus->setIcon(StyleHint::Minus, Size(32, 32));
    _minus->sigClicked.connect(sigc::mem_fun(this, &SpinBox::decrement));
    addChild(_minus);

    _plus = new ToolButton("+");
    _plus->setRepeatable(true);
    _plus->setToolButtonStyle(ToolButton::IconOnly);
    _plus->setIcon(StyleHint::Plus, Size(32, 32));
    _plus->sigClicked.connect(sigc::mem_fun(this, &SpinBox::increment));
    if (_max == _value)
        _plus->setDisabled();
    addChild(_plus);

    std::stringstream ss;
    ss << _prefix << _value << _postfix;
    _layout.setSingleLine(true);
    _layout.setText(ss.str());
    _layout.setAlignment(TextLayout::Center);

    sigGeometryUpdated.connect(sigc::mem_fun(this, &SpinBox::updateSpinBoxGeometry));
}

SpinBox::~SpinBox()
{
}

Size
SpinBox::preferredSize() const
{
    ILOG_TRACE_W(ILX_SPINBOX);
    Size plus = _plus->preferredSize();
    Size minus = _minus->preferredSize();
    Size text = stylist()->defaultFont(StyleHint::InputFont)->extents(_layout.text());
    ILOG_DEBUG(ILX_SPINBOX, " -> plus: %d, %d\n", plus.width(), plus.height());
    ILOG_DEBUG(ILX_SPINBOX, " -> minus: %d, %d\n", minus.width(), minus.height());
    ILOG_DEBUG(ILX_SPINBOX, " -> text: %d, %d\n", text.width(), text.height());
    return Size(text.width() + 20 + plus.width() + minus.width(), std::max(plus.height(), text.height() + stylist()->defaultParameter(StyleHint::LineInputTB)));
}

int
SpinBox::max() const
{
    return _max;
}

int
SpinBox::min() const
{
    return _min;
}

int
SpinBox::step() const
{
    return _step;
}

int
SpinBox::value() const
{
    return _value;
}

const std::string&
SpinBox::postfix() const
{
    return _postfix;
}

const std::string&
SpinBox::prefix() const
{
    return _prefix;
}

bool
SpinBox::wrapping() const
{
    return _wrapping;
}

TextLayout
SpinBox::layout() const
{
    return _layout;
}

void
SpinBox::setMax(int max)
{
    _max = max;

    if (_value == _max)
        _plus->setDisabled();
    else
        _plus->setEnabled();
    doLayout();
}

void
SpinBox::setMin(int min)
{
    _min = min;

    if (_value == _min)
        _minus->setDisabled();
    else
        _minus->setEnabled();
    doLayout();
}

void
SpinBox::setRange(int min, int max)
{
    setMin(min);
    setMax(max);
}

void
SpinBox::setStep(int step)
{
    _step = step;
}

void
SpinBox::setValue(int value)
{
    ILOG_TRACE_W(ILX_SPINBOX);
    if (value != _value)
    {
        ILOG_DEBUG(ILX_SPINBOX, " -> value: %d\n", value);

        if (value < _min)
            value = _min;
        if (value > _max)
            value = _max;

        _value = value;

        if (_value == _min)
            _minus->setDisabled();
        else
            _minus->setEnabled();

        if (_value == _max)
            _plus->setDisabled();
        else
            _plus->setEnabled();

        std::stringstream ss;
        ss << _prefix << _value << _postfix;
        _layout.setText(ss.str());

        doLayout();
        sigValueChanged(_value);
        update();
    }
}

void
SpinBox::setWrapping(bool wrapping)
{
    _wrapping = wrapping;
}

void
SpinBox::setPostfix(const std::string& postfix)
{
    if (_postfix != postfix)
    {
        _postfix = postfix;
        std::stringstream ss;
        ss << _prefix << _value << _postfix;
        _layout.setText(ss.str());
        doLayout();
        update();
    }
}

void
SpinBox::setPrefix(const std::string& prefix)
{
    if (_prefix != prefix)
    {
        _prefix = prefix;
        std::stringstream ss;
        ss << _prefix << _value << _postfix;
        _layout.setText(ss.str());
        doLayout();
        update();
    }
}

void
SpinBox::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    stylist()->drawSpinBox(&p, this);
    p.end();
}

void
SpinBox::decrement()
{
    setValue(_value - _step);
}

void
SpinBox::increment()
{
    setValue(_value + _step);
}

void
SpinBox::updateSpinBoxGeometry()
{
    ILOG_TRACE_W(ILX_SPINBOX);
    Size plus = _plus->preferredSize();
    Size minus = _minus->preferredSize();
    Font* font = stylist()->defaultFont(StyleHint::InputFont);
    Size text = font->extents(_layout.text());

    ILOG_DEBUG(ILX_SPINBOX, " -> plus: %d, %d\n", plus.width(), plus.height());
    ILOG_DEBUG(ILX_SPINBOX, " -> minus: %d, %d\n", minus.width(), minus.height());
    ILOG_DEBUG(ILX_SPINBOX, " -> text: %d, %d\n", text.width(), text.height());

    _minus->setGeometry(0, 0, plus.width(), plus.height());
    _minus->setNeighbours(getNeighbour(Up), getNeighbour(Down), getNeighbour(Left), _plus);
    _plus->setGeometry(width() - minus.width(), 0, minus.width(), minus.height());
    _plus->setNeighbours(getNeighbour(Up), getNeighbour(Down), _minus, getNeighbour(Right));
    _layout.setBounds(_plus->width(), (height() - text.height()) / 2, width() - _plus->width() - _minus->width(), text.height());
    _layout.doLayout(font);
}

} /* namespace ilixi */
