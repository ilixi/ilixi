/*
 Copyright 2011 Tarik Sekmen.

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

#include "ScrollItem.h"
#include "graphics/Painter.h"

using namespace ilixi;

ScrollItem::ScrollItem(const std::string& text, Widget* parent) :
    Widget(parent), TextBase(text, this), _val1(0)
{
  setConstraints(MinimumConstraint, FixedConstraint);
  setInputMethod(KeyAndPointerInput);
}

ScrollItem::~ScrollItem()
{
}

Size
ScrollItem::preferredSize() const
{
  return Size(200, 50);
}

void
ScrollItem::setImage(Image* image)
{
  _image = image;
}

void
ScrollItem::tweenSlot()
{
  repaint();
}

void
ScrollItem::focusInEvent()
{
  update();
}

void
ScrollItem::focusOutEvent()
{
  update();
}

void
ScrollItem::enterEvent(const PointerEvent& event)
{
  update();
}

void
ScrollItem::leaveEvent(const PointerEvent& event)
{
  update();
}

void
ScrollItem::pointerButtonDownEvent(const PointerEvent& event)
{
  sigPressed(_layout.text());
}

void
ScrollItem::compose(const Rectangle& rect)
{
  Painter p(this);
  p.begin(rect);

  // draw frame
  Color(51, 128, 204);
  if (state() & FocusedState)
    p.setBrush(Color(51, 128, 204));
  else if (state() & ExposedState)
    p.setBrush(Color(127, 127, 127));
  else
    p.setBrush(Color(255, 255, 255));
  p.fillRectangle(0, 0, width(), height());

  p.drawImage(_image, 0, 0, 50, 50);

  p.setBrush(Color(0, 0, 0));
  p.drawLayout(_layout, 50, 0);

  p.end();
}

void
ScrollItem::updateTextLayoutGeometry()
{
  _layout.setBounds(50, 0, width() - 30, height());
  _layout.doLayout(font());
}
