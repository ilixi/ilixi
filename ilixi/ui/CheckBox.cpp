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

#include <ui/CheckBox.h>
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_CHECKBOX, "ilixi/ui/CheckBox", "CheckBox");

CheckBox::CheckBox(const std::string& text, Widget* parent)
        : Button(text, parent)
{
    _layout.setSingleLine(true);
    setCheckable(true);
    setConstraints(MinimumConstraint, FixedConstraint);
    ILOG_TRACE_W(ILX_CHECKBOX);
}

CheckBox::~CheckBox()
{
    ILOG_TRACE_W(ILX_CHECKBOX);
}

int
CheckBox::heightForWidth(int width) const
{
    ILOG_TRACE_W(ILX_CHECKBOX);
    int w = stylist()->defaultParameter(StyleHint::CheckBoxWidth);
    int h = stylist()->defaultParameter(StyleHint::CheckBoxHeight);

    return std::max(textLayoutHeightForWidth(width - w), h);
}

Size
CheckBox::preferredSize() const
{
    ILOG_TRACE_W(ILX_CHECKBOX);
    int w = stylist()->defaultParameter(StyleHint::CheckBoxWidth);
    int h = stylist()->defaultParameter(StyleHint::CheckBoxHeight);

    if (!text().empty())
    {
        Size s = textExtents();
        w += stylist()->defaultParameter(StyleHint::ButtonOffset) + s.width();
        h = std::max(s.height(), h);
    }
    return Size(w, h);
}

bool
CheckBox::partial() const
{
    return (_buttonFlag & Partial);
}

void
CheckBox::toggleChecked()
{
    if (checkable())
    {
        if (_buttonFlag & Tristate)
        {
            if (checked())
            {
                setChecked(false);
                _buttonFlag = (ButtonFlags) (_buttonFlag | Partial);
                update();
                sigCheckStateChanged(Partial);
            } else if (partial())
            {
                _buttonFlag = (ButtonFlags) (_buttonFlag & ~Partial);
                update();
                sigCheckStateChanged(Unchecked);
            } else
            {
                setChecked(true);
                update();
                sigCheckStateChanged(Checked);
            }
        } else
        {
            if (checked())
            {
                setChecked(false);
                update();
                sigCheckChanged(false);
                sigCheckStateChanged(Unchecked);
            } else
            {
                setChecked(true);
                update();
                sigCheckChanged(true);
                sigCheckStateChanged(Checked);
            }
        }
    }
}

void
CheckBox::setTriState(bool triState)
{
    if (triState)
        _buttonFlag = (ButtonFlags) (_buttonFlag | Tristate);
    else
        _buttonFlag = (ButtonFlags) (_buttonFlag & ~Tristate);
}

void
CheckBox::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_CHECKBOX);
    Painter p(this);
    p.begin(event);
    stylist()->drawCheckBox(&p, this);
    p.end();
}

void
CheckBox::updateTextBaseGeometry()
{
    ILOG_TRACE_W(ILX_CHECKBOX);
    int x = stylist()->defaultParameter(StyleHint::ButtonOffset)
            + stylist()->defaultParameter(StyleHint::CheckBoxWidth);

    int th = textLayoutHeightForWidth(width() - x);
    int y = (height() - th) / 2;

    _layout.setBounds(x, y, width() - x, height());
    _layout.doLayout(font());
}

Font*
CheckBox::defaultFont() const
{
    return stylist()->defaultFont(StyleHint::DefaultFont);
}

} /* namespace ilixi */
