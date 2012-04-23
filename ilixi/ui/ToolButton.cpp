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

#include "ui/ToolButton.h"
#include "graphics/Painter.h"
#include "core/Logger.h"

using namespace ilixi;

ToolButton::ToolButton(std::string text, Widget* parent) :
    Button(text, parent), _toolButtonStyle(IconBeforeText)
{
  setConstraints(FixedConstraint, FixedConstraint);
  _layout.setSingleLine(true);
}

ToolButton::~ToolButton()
{
  ILOG_DEBUG(ILX, "~ToolButton %p\n", this);
}

Size
ToolButton::preferredSize() const
{
  Size defaultSize = stylist()->defaultSize(StyleHint::PushButton);

  if (text().empty() && !icon())
    return defaultSize;

  int hBorder = std::max(stylist()->defaultParameter(StyleHint::BorderWidth),
      stylist()->defaultParameter(StyleHint::ButtonRadius));
  int vBorder = stylist()->defaultParameter(StyleHint::BorderWidth);

  int w = 2 * hBorder;
  int h = 2 * vBorder;

  if (checkable())
    {
      if ((_toolButtonStyle == IconBelowText)
          || (_toolButtonStyle == IconAboveText))
        h += stylist()->defaultParameter(StyleHint::ButtonIndicator)
            + stylist()->defaultParameter(StyleHint::ButtonOffset);
      else
        w = hBorder + vBorder
            + stylist()->defaultParameter(StyleHint::ButtonIndicator)
            + stylist()->defaultParameter(StyleHint::ButtonOffset);
    }

  if (_toolButtonStyle == TextOnly)
    {
      Size s = textExtents();
      if (s.isValid())
        return Size(w + s.width(), h + s.height());
      return defaultSize;
    }

  // calculate image size
  int imgW = 0;
  int imgH = 0;
  if (icon())
    {
      imgW = icon()->width();
      imgH = icon()->height() + 2; // 1px for button down movement
    }

  if (_toolButtonStyle == IconOnly)
    {
      if (imgW)
        return Size(w + imgW, h + imgH);
      return defaultSize;
    }
  else
    {
      Size s = textExtents();
      if (s.isValid() && imgW)
        {
          if (_toolButtonStyle == IconBeforeText)
            {
              w += imgW + stylist()->defaultParameter(StyleHint::ButtonOffset)
                  + s.width();
              h += std::max(s.height(), imgH);
              return Size(w, h);
            }
          else
            {
              // IconBelowText or IconAboveText
              w += std::max(imgW, s.width());
              h += imgH + s.height()
                  + stylist()->defaultParameter(StyleHint::ButtonOffset);
              return Size(w, h);
            }
        }
      else if (imgW)
        return Size(w + imgW, h + imgH);
      else
        return Size(w + s.width(), h + s.height());
    }
}

ToolButton::ToolButtonStyle
ToolButton::getToolButtonStyle() const
{
  return _toolButtonStyle;
}

void
ToolButton::setToolButtonStyle(ToolButtonStyle style)
{
  _toolButtonStyle = style;
}

void
ToolButton::compose()
{
  Painter p(this);
  p.begin();
  stylist()->drawToolButton(&p, this);
  p.end();
}

void
ToolButton::updateTextLayoutGeometry()
{
  // Fixme align icon vertically.
  int textHeight = textExtents().height();
  int iconW = 0;
  int iconH = 0;
  int hBorder = std::max(stylist()->defaultParameter(StyleHint::BorderWidth),
      stylist()->defaultParameter(StyleHint::ButtonRadius));
  int vBorder = stylist()->defaultParameter(StyleHint::BorderWidth);
  int wUsed = 2 * hBorder;
  int x = hBorder;

  if (checkable())
    {
      if ((_toolButtonStyle == TextOnly) || (_toolButtonStyle == IconOnly)
          || (_toolButtonStyle == IconBeforeText))
        {
          x = vBorder + stylist()->defaultParameter(StyleHint::ButtonIndicator)
              + stylist()->defaultParameter(StyleHint::ButtonOffset);
          wUsed = x + hBorder;
        }
    }

  if (icon())
    {
      iconW = icon()->width();
      iconH = icon()->height() + 1;
    }

  if (_toolButtonStyle == TextOnly)
    {
      _layout.setBounds(x, (height() - textHeight) / 2, width() - wUsed,
          textHeight);
      return;
    }

  else if (_toolButtonStyle == IconOnly)
    {
      _icon->moveTo(x + (width() - (iconW + wUsed)) / 2,
          (height() - iconH) / 2 + 1);
      _layout.setBounds(0, 0, 0, 0);
      return;
    }

  else if (_toolButtonStyle == IconBeforeText)
    {
      if (iconW)
        {
          _icon->moveTo(x, vBorder + 1);
          x += iconW + stylist()->defaultParameter(StyleHint::ButtonOffset);
          wUsed += iconW + stylist()->defaultParameter(StyleHint::ButtonOffset);
        }
      _layout.setBounds(x, (height() - textHeight) / 2, width() - wUsed,
          textHeight);
    }
  else if (_toolButtonStyle == IconBelowText)
    {
      Size s = textExtents();
      x = (width() - wUsed - s.width()) / 2;
      _layout.setBounds(x, vBorder, width() - wUsed, textHeight);
      if (iconW)
        {
          _icon->moveTo((width() - iconW) / 2,
              vBorder + textHeight + 1
                  + stylist()->defaultParameter(StyleHint::ButtonOffset));
        }
    }
  else //  IconAboveText
    {
      int y = vBorder;
      if (iconW)
        {
          _icon->moveTo((width() - iconW) / 2, y + 1);
          y += iconH + stylist()->defaultParameter(StyleHint::ButtonOffset) + 1;
        }

      Size s = textExtents();
      x = (width() - s.width()) / 2;

      _layout.setBounds(x, y, width() - x, textHeight);
    }
  _layout.doLayout(font());
}
