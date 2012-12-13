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

#include <ui/Label.h>
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_LABEL, "ilixi/ui/Label", "Label");

Label::Label(std::string text, Widget* parent)
        : Widget(parent),
          TextBase("", this),
          _margin(0)
{
    setText(text);
    setConstraints(MinimumConstraint, MinimumConstraint);
    setSingleLine(true);
    ILOG_TRACE_W(ILX_LABEL);
}

Label::Label(const Label& label)
        : TextBase(label),
          _margin(label._margin)
{
    ILOG_TRACE_W(ILX_LABEL);
}

Label::~Label()
{
    ILOG_TRACE_W(ILX_LABEL);
}

int
Label::heightForWidth(int width) const
{
    ILOG_TRACE_W(ILX_LABEL);
    return textLayoutHeightForWidth(width - _margin.hSum()) + _margin.vSum();
}

Size
Label::preferredSize() const
{
    ILOG_TRACE_W(ILX_LABEL);
    Size s = textExtents();
    return Size(s.width() + _margin.hSum(), +s.height() + _margin.vSum());
}

Margin
Label::margin() const
{
    return _margin;
}

void
Label::setMargin(const Margin& margin)
{
    _margin = margin;
}

void
Label::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_LABEL);
    Painter p(this);
    p.begin(event);
    stylist()->drawLabel(&p, this);
    p.end();
}

void
Label::updateTextBaseGeometry()
{
    ILOG_TRACE_W(ILX_LABEL);
    _layout.setBounds(_margin.right(), _margin.top(), width() - _margin.hSum(), height() - _margin.vSum());
    _layout.doLayout(font());
}

} /* namespace ilixi */
