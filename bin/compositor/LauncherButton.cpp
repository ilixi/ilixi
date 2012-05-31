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

#include "LauncherButton.h"
#include "graphics/Painter.h"

namespace ilixi
{

  LauncherButton::LauncherButton(const std::string& name, Widget* parent) :
      ToolButton(name, parent)
  {
    setInputMethod(KeyAndPointerInput);
    setToolButtonStyle(ToolButton::IconAboveText);
  }

  LauncherButton::~LauncherButton()
  {
  }

  void
  LauncherButton::compose(const PaintEvent& event)
  {
    Painter p(this);
    p.begin(event.rect);
    if (state() & FocusedState)
      p.setBrush(Color(51, 102, 255));
    else if (state() & PressedState)
      p.setBrush(Color(128, 128, 128));
    else
      p.setBrush(Color(255, 255, 255));
    p.setFont(*font());
    p.drawLayout(_layout);
    p.end();
  }

} /* namespace ilixi */
