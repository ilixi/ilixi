/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#include "ui/Frame.h"
#include "graphics/Painter.h"
#include "core/Logger.h"

using namespace ilixi;

Frame::Frame(Widget* parent) :
    BorderBase(parent), ContainerBase(parent), _backgroundFilled(true)
{
  setBorderWidth(1);
  setBorderStyle(StyledBorder);
  setConstraints(NoConstraint, MinimumExpandingConstraint);
}

Frame::~Frame()
{
  ILOG_DEBUG(ILX_FRAME, "~Frame %p\n", this);
}

int
Frame::heightForWidth(int width) const
{
  return _layout->heightForWidth(
      width - (_canvasTopLeft.x() + _margin.hSum() + 2 * borderOffset()))
      + _canvasTopLeft.y() + _margin.vSum() + 2 * borderWidth();
}

Size
Frame::preferredSize() const
{
  Size s = _layout->preferredSize();
  return Size(
      s.width() + _canvasTopLeft.x() + _margin.hSum() + 2 * borderOffset(),
      s.height() + _canvasTopLeft.y() + _margin.vSum() + 2 * borderWidth());
}

bool
Frame::backgroundFilled() const
{
  return _backgroundFilled;
}

Margin
Frame::margin() const
{
  return _margin;
}

void
Frame::setBackgroundFilled(bool fill)
{
  _backgroundFilled = fill;
}

void
Frame::setCanvasPosition(const Point& topLeft)
{
  _canvasTopLeft = topLeft;
  doLayout();
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
  _layout->setGeometry(canvasX(), canvasY(), canvasWidth(), canvasHeight());
}

void
Frame::compose()
{
  Painter painter(this);
  painter.begin();
  stylist()->drawFrame(&painter, this);
  painter.end();
}

int
Frame::canvasX() const
{
  return _canvasTopLeft.x() + _margin.left() + borderOffset();
}

int
Frame::canvasY() const
{
  return _canvasTopLeft.y() + _margin.top() + borderWidth();
}

int
Frame::canvasHeight() const
{
  return height() - (_canvasTopLeft.y() + _margin.vSum()) - 2 * borderWidth();
}

int
Frame::canvasWidth() const
{
  return width() - (_canvasTopLeft.x() + _margin.hSum()) - 2 * borderOffset();
}
