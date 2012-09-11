/*
 * SpinBox.cpp
 *
 *  Created on: Sep 9, 2012
 *      Author: tarik
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
          _min(0),
          _max(value > 100 ? value : 100),
          _value(value),
          _step(1),
          _wrapping(true)
{
    ILOG_TRACE_W(ILX_SPINBOX);
    setConstraints(FixedConstraint, FixedConstraint);
    setInputMethod(PointerInput);

    _plus = new ToolButton("+");
    _plus->setRepeatable(true);
    _plus->setDrawFrame(false);
    _plus->setToolButtonStyle(ToolButton::IconOnly);
    _plus->setIcon(StyleHint::Plus);
    _plus->sigClicked.connect(sigc::mem_fun(this, &SpinBox::increment));
    if (_max == _value)
        _plus->setDisabled();
    addChild(_plus);

    _minus = new ToolButton("-");
    _minus->setRepeatable(true);
    _minus->setDrawFrame(false);
    _minus->setToolButtonStyle(ToolButton::IconOnly);
    _minus->setIcon(StyleHint::Minus);
    _minus->sigClicked.connect(sigc::mem_fun(this, &SpinBox::decrement));
    addChild(_minus);

    std::stringstream ss;
    ss << _prefix << _value << _postfix;
    _layout.setSingleLine(true);
    _layout.setText(ss.str());
    _layout.setAlignment(TextLayout::Center);

    sigGeometryUpdated.connect(
            sigc::mem_fun(this, &SpinBox::updateSpinBoxGeometry));
}

SpinBox::~SpinBox()
{
}

Size
SpinBox::preferredSize() const
{
    Font* font = stylist()->defaultFont(StyleHint::InputFont);
    std::stringstream ss;
    ss << _prefix << _value << _postfix;
    Size text = font->extents(ss.str());
    return Size(text.width() + 80, _plus->preferredSize().height());
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
}

void
SpinBox::setMin(int min)
{
    _min = min;

    if (_value == _min)
        _minus->setDisabled();
    else
        _minus->setEnabled();
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
    _minus->setGeometry(0, 0, 40, height());
    _plus->setGeometry(width() - 40, 0, 40, height());
    Size s = stylist()->defaultFont(StyleHint::InputFont)->extents(
            _layout.text());
    int y = (height() - s.height()) / 2;
    _layout.setBounds(40, y, width() - 80, s.height());
    _layout.doLayout(stylist()->defaultFont(StyleHint::InputFont));
}

} /* namespace ilixi */
