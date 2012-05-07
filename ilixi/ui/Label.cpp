/*
 Copyright 2012 Tarik Sekmen.

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

#include "ui/Label.h"
#include "graphics/Painter.h"
#include "core/Logger.h"

using namespace ilixi;

Label::Label(std::string text, Widget* parent) :
  Widget(parent), BorderBase(this), TextBase(text, this), _margin(0)
{
  setConstraints(NoConstraint, MinimumConstraint);
}

Label::Label(const Label& label) :
  BorderBase(label), TextBase(label), _margin(label._margin)
{
}

Label::~Label()
{
}

int
Label::heightForWidth(int width) const
{
  return textLayoutHeightForWidth(width - (2 * borderOffset() + _margin.hSum()))
      + 2 * borderWidth() + _margin.vSum();
}

Size
Label::preferredSize() const
{
  Size s = textExtents();
  return Size(2 * borderOffset() + s.width() + _margin.hSum(),
      2 * borderWidth() + s.height() + _margin.vSum());
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
Label::compose()
{
  Painter p(this);
  p.begin();
  stylist()->drawFrame(&p, this);
  stylist()->drawLabel(&p, this);
  p.end();
}

void
Label::updateTextLayoutGeometry()
{
  _layout.setBounds(_margin.right() + borderOffset(),
      _margin.top() + borderWidth(),
      width() - (2 * borderOffset() + _margin.hSum()),
      height() - (2 * borderWidth() + _margin.vSum()));
  _layout.doLayout(font());
}
