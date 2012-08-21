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

#include <ui/PushButton.h>
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_PUSHBUTTON, "ilixi/ui/PushButton", "PushButton");

PushButton::PushButton(const std::string& text, Widget* parent)
        : Button(text, parent)
{
    ILOG_TRACE_W(ILX_PUSHBUTTON);
    setConstraints(MinimumConstraint, FixedConstraint);
    _layout.setSingleLine(true);
    _layout.setAlignment(TextLayout::Center);
}

PushButton::~PushButton()
{
    ILOG_TRACE_W(ILX_PUSHBUTTON);
}

Size
PushButton::preferredSize() const
{
    ILOG_TRACE_W(ILX_PUSHBUTTON);

    if (text().empty())
        return stylist()->defaultSize(StyleHint::PushButton);

    int w = stylist()->defaultParameter(StyleHint::PushButtonCorners);

    if (!text().empty())
    {
        Size s = textExtents();
        w += s.width();
    }
    return Size(w, stylist()->defaultParameter(StyleHint::PushButtonHeight));
}

void
PushButton::focusInEvent()
{
//  stylist()->animate(Stylist::FocusIn, this);
    update();
}

void
PushButton::focusOutEvent()
{
//  stylist()->animate(Stylist::FocusOut, this);
    update();
}

void
PushButton::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_PUSHBUTTON);
    Painter p(this);
    p.begin(event);
    stylist()->drawPushButton(&p, this);
    p.end();
}

void
PushButton::updateTextBaseGeometry()
{
    ILOG_TRACE(ILX_PUSHBUTTON);

    int textHeight = textExtents().height();
    int y = (height() - textHeight) / 2;

    _layout.setBounds(
            stylist()->defaultParameter(StyleHint::PushButtonCorners), y,
            width() - 2 * stylist()->defaultParameter(StyleHint::PushButtonCorners),
            textHeight);
    _layout.doLayout(font());
}

} /* namespace ilixi */
