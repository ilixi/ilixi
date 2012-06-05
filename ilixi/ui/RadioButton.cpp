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

#include "ui/RadioButton.h"
#include "graphics/Painter.h"

using namespace ilixi;

RadioButton::RadioButton(const std::string& text, Widget* parent) :
    Button(text, parent)
{
  _layout.setSingleLine(true);
  setCheckable(true);
  setConstraints(NoConstraint, MinimumConstraint);
  sigCheckChanged.connect(sigc::mem_fun(this, &RadioButton::informGroup));
}

RadioButton::~RadioButton()
{
}

int
RadioButton::heightForWidth(int width) const
{
  int w = stylist()->defaultParameter(StyleHint::RadioOffset)
      + stylist()->defaultParameter(StyleHint::RadioWidth);
  int h = stylist()->defaultParameter(StyleHint::RadioHeight);

  if (icon())
    {
      w += stylist()->defaultParameter(StyleHint::RadioOffset)
          + icon()->width();
      h = std::max(icon()->height(), h);
    }

  return std::max(textLayoutHeightForWidth(width - w), h);
}

Size
RadioButton::preferredSize() const
{
  int w = stylist()->defaultParameter(StyleHint::RadioOffset)
      + stylist()->defaultParameter(StyleHint::RadioWidth);
  int h = stylist()->defaultParameter(StyleHint::RadioHeight);

  if (icon())
    {
      w += stylist()->defaultParameter(StyleHint::RadioOffset)
          + icon()->width();
      h = std::max(icon()->height(), h);
    }

  if (!text().empty())
    {
      Size s = textExtents();
      w += stylist()->defaultParameter(StyleHint::RadioOffset) + s.width();
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
  _group->select(this);
}

void
RadioButton::compose(const PaintEvent& event)
{
  Painter p(this);
  p.begin(event);
  stylist()->drawRadioButton(&p, this);
  p.end();
}

void
RadioButton::updateTextBaseGeometry()
{
  int x = 2 * stylist()->defaultParameter(StyleHint::RadioOffset)
      + stylist()->defaultParameter(StyleHint::RadioWidth);

  if (icon())
    {
      _icon->moveTo(x, (height() - icon()->height()) / 2);
      x += stylist()->defaultParameter(StyleHint::RadioOffset)
          + icon()->width();
    }

  int th = textLayoutHeightForWidth(width() - x);
  int y = (height() - th) / 2;

  _layout.setBounds(x, y, width() - x, height());
  _layout.doLayout(font());
}
