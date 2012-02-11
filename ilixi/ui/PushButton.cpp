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

#include "ui/PushButton.h"
#include "graphics/Painter.h"

using namespace ilixi;

PushButton::PushButton(const std::string& text, Widget* parent) :
    Button(text, parent)
{
  setConstraints(MinimumConstraint, FixedConstraint);
  _layout.setSingleLine(true);
}

PushButton::~PushButton()
{
}

Size
PushButton::preferredSize() const
{
  if (text().empty() && !icon())
    return stylist()->defaultSize(StyleHint::PushButton);

  int border = std::max(stylist()->defaultParameter(StyleHint::BorderWidth),
      stylist()->defaultParameter(StyleHint::ButtonRadius));

  int w = 2 * border;
  int h = 0;

  if (icon())
    {
      w += icon()->width()
          + stylist()->defaultParameter(StyleHint::ButtonOffset);
      h += icon()->height() + 2; // 2px for button pressed state
    }

  if (!text().empty())
    {
      Size s = textExtents();
      w += s.width();
      h = std::max(s.height(), h)
          + 2 * stylist()->defaultParameter(StyleHint::BorderWidth);
    }
  return Size(w, h);
}

void
PushButton::focusInEvent()
{
  stylist()->animate(Stylist::FocusIn, this);
}

void
PushButton::focusOutEvent()
{
  stylist()->animate(Stylist::FocusOut, this);
}

void
PushButton::compose()
{
  Painter p(this);
  p.begin();
  stylist()->drawPushButton(&p, this);
  p.end();
}

void
PushButton::updateTextLayoutGeometry()
{
  int border = std::max(stylist()->defaultParameter(StyleHint::BorderWidth),
      stylist()->defaultParameter(StyleHint::ButtonRadius));

  int x = border;
  int textHeight = textExtents().height();
  int y = (height() - textHeight) / 2;
  int w = 2 * border;

  if (icon())
    {
      _icon->moveTo(x, (height() - icon()->height()) / 2);
      x += icon()->width()
          + stylist()->defaultParameter(StyleHint::ButtonOffset);
      w = x + border;
    }

  _layout.setBounds(x, y, width() - w, textHeight);
  _layout.doLayout(font());
}
