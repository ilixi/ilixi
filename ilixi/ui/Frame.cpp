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

#include <ui/Frame.h>
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_FRAME, "ilixi/ui/Frame", "Frame");

Frame::Frame(Widget* parent)
        : ContainerBase(parent),
          _margin(0),
          _drawFrame(true)
{
    setConstraints(MinimumConstraint, MinimumExpandingConstraint);
}

Frame::~Frame()
{
    ILOG_TRACE_W(ILX_FRAME);
}

int
Frame::heightForWidth(int width) const
{
    if (_drawFrame)
        return _layout->heightForWidth(width - _margin.hSum() - stylist()->defaultParameter(StyleHint::FrameOffsetLR)) + _margin.vSum() + stylist()->defaultParameter(StyleHint::FrameOffsetLR);
    return _layout->heightForWidth(width - _margin.hSum()) + _margin.vSum();
}

Size
Frame::preferredSize() const
{
    Size s = _layout->preferredSize();
    if (_drawFrame)
        return Size(s.width() + _margin.hSum() + stylist()->defaultParameter(StyleHint::FrameOffsetLR), s.height() + _margin.vSum() + stylist()->defaultParameter(StyleHint::FrameOffsetTB));
    return Size(s.width() + _margin.hSum(), s.height() + _margin.vSum());
}

int
Frame::canvasX() const
{
    if (_drawFrame)
        return _margin.left() + stylist()->defaultParameter(StyleHint::FrameOffsetLeft);
    return _margin.left();
}

int
Frame::canvasY() const
{
    if (_drawFrame)
        return _margin.top() + stylist()->defaultParameter(StyleHint::FrameOffsetTop);
    return _margin.top();
}

int
Frame::canvasHeight() const
{
    if (_drawFrame)
        return height() - _margin.vSum() - stylist()->defaultParameter(StyleHint::FrameOffsetTB);
    return height() - _margin.vSum();
}

int
Frame::canvasWidth() const
{
    if (_drawFrame)
        return width() - _margin.hSum() - stylist()->defaultParameter(StyleHint::FrameOffsetLR);
    return width() - _margin.hSum();
}

Margin
Frame::margin() const
{
    return _margin;
}

void
Frame::setDrawFrame(bool drawFrame)
{
    _drawFrame = drawFrame;
}

void
Frame::setMargins(int top, int bottom, int left, int right)
{
    _margin.setMargins(top, bottom, left, right);
    doLayout();
}

void
Frame::setMargin(const Margin& margin)
{
    _margin = margin;
    doLayout();
}

void
Frame::updateLayoutGeometry()
{
    ILOG_TRACE_W(ILX_FRAME);
    ILOG_DEBUG( ILX_FRAME, " -> R(%d, %d, %d, %d)\n", canvasX(), canvasY(), canvasWidth(), canvasHeight());
    _layout->setGeometry(canvasX(), canvasY(), canvasWidth(), canvasHeight());
}

void
Frame::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_FRAME);
    if (_drawFrame)
    {
        Painter painter(this);
        painter.begin(event);
        stylist()->drawFrame(&painter, this);
        painter.end();
    }
}

} /* namespace ilixi */
