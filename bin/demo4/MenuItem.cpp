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

#include "MenuItem.h"
#include "graphics/Painter.h"

namespace ilixi
{

  MenuItem::MenuItem(const std::string& text, Widget* parent) :
      Button(text, parent), _image(NULL), _val1(0)
  {
    setConstraints(FixedConstraint, FixedConstraint);
    setInputMethod(KeyAndPointerInput);

//    _image = new Image(ILIXI_DATADIR"images/default.png", 196, 196);

    _inAni = new TweenAnimation();
    _inAni->addTween(Tween::CIRCLE, Tween::EASE_OUT, _val1, 0, 1);
    _inAni->setDuration(500);
    _inAni->sigExec.connect(sigc::mem_fun(this, &MenuItem::tweenSlot));

    _outAni = new TweenAnimation();
    _outAni->addTween(Tween::SINE, Tween::EASE_OUT, _val1, 1, 0);
    _outAni->setDuration(300);
    _outAni->sigExec.connect(sigc::mem_fun(this, &MenuItem::tweenSlot));

//    setLayoutAlignment(TextLayout::Center);
  }

  MenuItem::~MenuItem()
  {
    delete _image;
  }

  Size
  MenuItem::preferredSize() const
  {
    return Size(196, 196);
  }

  Image*
  MenuItem::image() const
  {
    return _image;
  }

  void
  MenuItem::setImage(Image* image)
  {
    delete _image;
    _image = image;
  }

  void
  MenuItem::tweenSlot()
  {
    update();
  }

  void
  MenuItem::focusInEvent()
  {
    sigFocused(this);
    _outAni->stop();
    _inAni->start();
  }

  void
  MenuItem::focusOutEvent()
  {
    _inAni->stop();
    _outAni->start();
  }

  void
  MenuItem::compose()
  {
    Painter p(this);
    p.begin();

    // draw image
    p.setBrush(Color(0, 0, 0, 155 + _val1 * 100));
    p.drawImage(_image, -20 * _val1, 40 - 20 * _val1, 196 + 40 * _val1,
        156 + 40 * _val1,
        (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_ALPHACHANNEL
            | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY));

    // overlay rect
    int y = height() - _val1 * 50;
    Color c("006666");
    c.setSaturation(_val1);
    p.setBrush(c);
    p.fillRectangle(0, y, width(), 50);

//    Color txt("333333");
//    txt.setBrightness(_val1);
//    p.setBrush(txt);

    p.setBrush(Color(255, 255, 255, 255));
    p.drawLayout(_layout, _layout.x(), _layout.y() - y);

    p.end();
  }

  void
  MenuItem::updateTextLayoutGeometry()
  {
    Size s = textExtents();
    int x = (width() - s.width()) / 2;
    _layout.setBounds(x, 155, width() - x, 56);
    _layout.doLayout(font());
  }

} /* namespace ilixi */
