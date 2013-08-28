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

#include <ui/RadioButton.h>
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_RADIOBUTTON, "ilixi/ui/RadioButton", "RadioButton");

RadioButton::RadioButton(const std::string& text, Widget* parent)
        : Button(text, parent),
          _group(NULL)
{
    ILOG_TRACE_W(ILX_RADIOBUTTON);
    _layout.setSingleLine(true);
    setCheckable(true);
    setConstraints(NoConstraint, FixedConstraint);
    sigCheckChanged.connect(sigc::mem_fun(this, &RadioButton::informGroup));
}

RadioButton::~RadioButton()
{
    ILOG_TRACE_W(ILX_RADIOBUTTON);
}

int
RadioButton::heightForWidth(int width) const
{
    int w = stylist()->defaultParameter(StyleHint::RadioWidth);
    int h = stylist()->defaultParameter(StyleHint::RadioHeight);

    return std::max(textLayoutHeightForWidth(width - w), h);
}

Size
RadioButton::preferredSize() const
{
    int w = stylist()->defaultParameter(StyleHint::RadioWidth);
    int h = stylist()->defaultParameter(StyleHint::RadioHeight);

    if (!text().empty())
    {
        Size s = textExtents();
        w += stylist()->defaultParameter(StyleHint::ButtonOffset) + s.width();
        h = std::max(s.height(), h);
    }
    return Size(w, h);
}

void
RadioButton::setGroup(RadioGroup* group)
{
    _group = group;
}

void
RadioButton::setChecked(bool checked)
{
    if (checked)
    {
        _buttonFlag = (ButtonFlags) (_buttonFlag | Checked);
        sigCheckChanged(true);
    } else
    {
        _buttonFlag = (ButtonFlags) (_buttonFlag & ~Checked);
        sigCheckChanged(false);
    }
    update();
}

void
RadioButton::toggleChecked()
{
    if (!checked())
    {
        setChecked();
        sigCheckChanged(true);
    }
    update();
}

void
RadioButton::informGroup(bool checked)
{
    if (_group)
        _group->select(this);
}

void
RadioButton::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_RADIOBUTTON);
    Painter p(this);
    p.begin(event);
    stylist()->drawRadioButton(&p, this);
    p.end();
}

void
RadioButton::updateTextBaseGeometry()
{
    ILOG_TRACE_W(ILX_RADIOBUTTON);
    int x = stylist()->defaultParameter(StyleHint::ButtonOffset) + stylist()->defaultParameter(StyleHint::RadioWidth);

    int th = textLayoutHeightForWidth(width() - x);
    int y = (height() - th) / 2;

    _layout.setBounds(x, y, width() - x, height());
    _layout.doLayout(font());
}

Font*
RadioButton::defaultFont() const
{
    return stylist()->defaultFont(StyleHint::DefaultFont);
}

} /* namespace ilixi */
