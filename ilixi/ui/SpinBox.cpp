/*
 * SpinBox.cpp
 *
 *  Created on: Sep 9, 2012
 *      Author: tarik
 */

#include <ui/SpinBox.h>
#include <sstream>

namespace ilixi
{

SpinBox::SpinBox(int value, Widget* parent)
        : Widget(parent),
          _lineInput(NULL),
          _plus(NULL),
          _minus(NULL),
          _min(0),
          _max(100),
          _value(value),
          _step(1),
          _wrapping(true)
{
    setConstraints(FixedConstraint, FixedConstraint);
    setInputMethod(PointerInput);

    std::stringstream ss;
    ss << _value;
    _lineInput = new LineInput(ss.str());
    _lineInput->setMargin(Margin(0, 0, 40, 40));
    addChild(_lineInput);

    _plus = new ToolButton("+");
    _plus->setRepeatable(true);
    _plus->setDrawFrame(false);
    _plus->setToolButtonStyle(ToolButton::IconOnly);
    _plus->setIcon(new Icon(StyleHint::Plus));
    _plus->sigClicked.connect(sigc::mem_fun(this, &SpinBox::increment));
    _plus->setDisabled();
    addChild(_plus);

    _minus = new ToolButton("-");
    _minus->setRepeatable(true);
    _minus->setDrawFrame(false);
    _minus->setToolButtonStyle(ToolButton::IconOnly);
    _minus->setIcon(new Icon(StyleHint::Minus));
    _minus->sigClicked.connect(sigc::mem_fun(this, &SpinBox::decrement));
    addChild(_minus);

    sigGeometryUpdated.connect(
            sigc::mem_fun(this, &SpinBox::updateSpinBoxGeometry));
}

SpinBox::~SpinBox()
{
}

Size
SpinBox::preferredSize() const
{
    Size s = _lineInput->preferredSize();
    return Size(s.width(), s.height());
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

bool
SpinBox::wrapping() const
{
    return _wrapping;
}

void
SpinBox::setMax(int max)
{
    _max = max;
}

void
SpinBox::setMin(int min)
{
    _min = min;
}

void
SpinBox::setRange(int min, int max)
{
    _min = min;
    _max = max;
}

void
SpinBox::setStep(int step)
{
    _step = step;
}

void
SpinBox::setValue(int value)
{
    if (value != _value && value >= _min && value <= _max)
    {
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
        ss << _value;
        _lineInput->setText(ss.str());
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
SpinBox::compose(const PaintEvent& event)
{
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
    _lineInput->setGeometry(0, 0, width(), height());
    _minus->setGeometry(0, 0, 40, height());
    _plus->setGeometry(width() - 40, 0, 40, height());
}

} /* namespace ilixi */
