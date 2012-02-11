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

#include "ui/CheckBox.h"
#include "graphics/Painter.h"
#include "core/Logger.h"

using namespace ilixi;

CheckBox::CheckBox(const std::string& text, Widget* parent) :
    Button(text, parent)
{
  _layout.setSingleLine(true);
  setCheckable(true);
  setConstraints(NoConstraint, MinimumConstraint);
}

CheckBox::~CheckBox()
{
}

int
CheckBox::heightForWidth(int width) const
{
  int w = stylist()->defaultParameter(StyleHint::CheckBoxOffset)
      + stylist()->defaultParameter(StyleHint::CheckBoxWidth);
  int h = stylist()->defaultParameter(StyleHint::CheckBoxHeight);

  if (icon())
    {
      w += stylist()->defaultParameter(StyleHint::CheckBoxOffset)
          + icon()->width();
      h = std::max(icon()->height(), h);
    }

  return std::max(textLayoutHeightForWidth(width - w), h);
}

Size
CheckBox::preferredSize() const
{
  int w = stylist()->defaultParameter(StyleHint::CheckBoxOffset)
      + stylist()->defaultParameter(StyleHint::CheckBoxWidth);
  int h = stylist()->defaultParameter(StyleHint::CheckBoxHeight);

  if (icon())
    {
      w += stylist()->defaultParameter(StyleHint::CheckBoxOffset)
          + icon()->width();
      h = std::max(icon()->height(), h);
    }

  if (!text().empty())
    {
      Size s = textExtents();
      w += stylist()->defaultParameter(StyleHint::CheckBoxOffset) + s.width();
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
              sigCheckStateChanged(Partial);
            }
          else if (partial())
            {
              _buttonFlag = (ButtonFlags) (_buttonFlag & ~Partial);
              sigCheckStateChanged(Unchecked);
            }
          else
            {
              setChecked(true);
              sigCheckStateChanged(Checked);
            }
        }
      else
        {
          if (checked())
            {
              setChecked(false);
              sigCheckChanged(false);
              sigCheckStateChanged(Unchecked);
            }
          else
            {
              setChecked(true);
              sigCheckChanged(true);
              sigCheckStateChanged(Checked);
            }
        }
      update();
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
CheckBox::compose()
{
  Painter p(this);
  p.begin();
  stylist()->drawCheckBox(&p, this);
  p.end();
}

void
CheckBox::updateTextLayoutGeometry()
{
  int x = 2 * stylist()->defaultParameter(StyleHint::CheckBoxOffset)
      + stylist()->defaultParameter(StyleHint::CheckBoxWidth);

  if (icon())
    {
      _icon->moveTo(x, (height() - icon()->height()) / 2);
      x += stylist()->defaultParameter(StyleHint::CheckBoxOffset)
          + icon()->width();
    }

  int th = textLayoutHeightForWidth(width() - x);
  int y = (height() - th) / 2;

  _layout.setBounds(x, y, width() - x, height());
  _layout.doLayout(font());
}
