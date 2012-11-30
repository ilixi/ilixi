/*
 Copyright 2010-2012 Tarik Sekmen.

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
      Button(text, parent), _image(NULL), _doOut(false)
  {
    setConstraints(FixedConstraint, FixedConstraint);
    setInputMethod(KeyPointer);

//    _image = new Image(ILIXI_DATADIR"images/default.png", 196, 196);

    _tweenIn = new Tween(Tween::CIRCLE, Tween::EASE_OUT, 0, 1);
    _inAni.addTween(_tweenIn);
    _inAni.setDuration(500);
    _inAni.sigExec.connect(sigc::mem_fun(this, &MenuItem::tweenSlot));

    _tweenOut = new Tween(Tween::SINE, Tween::EASE_OUT, 1, 0);
    _outAni.addTween(_tweenOut);
    _outAni.setDuration(300);
    _outAni.sigExec.connect(sigc::mem_fun(this, &MenuItem::tweenSlot));

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
    _outAni.stop();
    _doOut = false;
    _inAni.start();
  }

  void
  MenuItem::focusOutEvent()
  {
    _inAni.stop();
    _doOut = true;
    _outAni.start();
  }

  void
  MenuItem::compose(const PaintEvent& event)
  {
    Painter p(this);
    p.begin(event);

    float val = _tweenIn->value();
    if (_doOut)
      val = _tweenOut->value();

    // draw image
    p.setBrush(Color(0, 0, 0, 155 + val * 100));
    p.stretchImage(_image, -20 * val, 40 - 20 * val, 196 + 40 * val,
        156 + 40 * val,
        (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_ALPHACHANNEL
            | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY));

    // overlay rect
    int y = height() - val * 50;
    Color c("006666");
    c.setSaturation(val);
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
  MenuItem::updateTextBaseGeometry()
  {
    Size s = textExtents();
    int x = (width() - s.width()) / 2;
    _layout.setBounds(x, 155, width() - x, 56);
    _layout.doLayout(font());
  }

} /* namespace ilixi */
