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

#include "ui/BorderBase.h"
#include "core/Logger.h"

using namespace ilixi;

BorderBase::BorderBase(Widget* owner) :
    _borderStyle(NoBorder), _borderWidth(0), _owner(owner)
{
  ILOG_TRACE(ILX_BORDERBASE);
}

BorderBase::BorderBase(const BorderBase& borderbase) :
    _borderStyle(borderbase._borderStyle), _borderWidth(
        borderbase._borderWidth), _owner(borderbase._owner)
{
  ILOG_TRACE(ILX_BORDERBASE);
}

BorderBase::~BorderBase()
{
  ILOG_TRACE(ILX_BORDERBASE);
}

WidgetBorderStyle
BorderBase::borderStyle() const
{
  return _borderStyle;
}

int
BorderBase::borderWidth() const
{
  if (_borderStyle == NoBorder)
    return 0;
  else if (_borderStyle == StyledBorder)
    return _owner->stylist()->defaultParameter(StyleHint::BorderWidth);
  else
    return _borderWidth;
}

int
BorderBase::borderOffset() const
{
  if (_borderStyle == NoBorder)
    return 0;
  else if (_borderStyle == StyledBorder)
    return std::max(_owner->stylist()->defaultParameter(StyleHint::BorderWidth),
        _owner->stylist()->defaultParameter(StyleHint::FrameBorderRadius));
  else
    return _borderWidth;
}

void
BorderBase::setBorderStyle(WidgetBorderStyle style)
{
  _borderStyle = style;
}

void
BorderBase::setBorderWidth(int borderWidth)
{
  _borderWidth = borderWidth;
}
