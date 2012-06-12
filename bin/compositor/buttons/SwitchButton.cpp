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

#include "SwitchButton.h"
#include "graphics/Painter.h"

namespace ilixi
{

  SwitchButton::SwitchButton(const std::string& imagePath, Widget* parent) :
      CompositorButton(imagePath, parent)
  {
  }

  SwitchButton::~SwitchButton()
  {
  }

  void
  SwitchButton::compose(const PaintEvent& event)
  {
    Painter p(this);
    p.begin(event);
    p.setBrush(Color(0, 0, 0, 255));
    p.drawImage(_image, -80 * _tween->value(), 80 * _tween->value());
    p.end();
  }

} /* namespace ilixi */
