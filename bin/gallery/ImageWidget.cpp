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

#include "ImageWidget.h"
#include "graphics/Painter.h"
#include "core/Logger.h"
#include "ui/PushButton.h"

using namespace ilixi;

ImageWidget::ImageWidget(const std::string& text, Widget* parent) :
    Button(text, parent), _doOut(false)
{
  _inAni.setDuration(500);
  _inAni.sigExec.connect(sigc::mem_fun(this, &ImageWidget::tweenSlot));
  _circleIn = new Tween(Tween::SINE, Tween::EASE_OUT, 0, 1);
  _inAni.addTween(_circleIn);
  _bounceIn = new Tween(Tween::BOUNCE, Tween::EASE_OUT, 0, 1);
  _inAni.addTween(_bounceIn);

  _outAni.setDuration(300);
  _outAni.sigExec.connect(sigc::mem_fun(this, &ImageWidget::tweenSlot));
  _circleOut = new Tween(Tween::SINE, Tween::EASE_OUT, 1, 0);
  _outAni.addTween(_circleOut);
  _bounceOut = new Tween(Tween::SINE, Tween::EASE_OUT, 1, 0);
  _outAni.addTween(_bounceOut);

  setInputMethod(KeyAndPointerInput);

  PushButton* button = new PushButton("text");
  Size s = button->preferredSize();
  button->setGeometry(10, 10, s.width(), s.height());
  addChild(button);
}

ImageWidget::~ImageWidget()
{
  delete _image;
}

void
ImageWidget::setImage(Image* image)
{
  _image = image;
}

void
ImageWidget::tweenSlot()
{
  update();
}

void
ImageWidget::compose(const PaintEvent& event)
{
  Painter p(this);
  p.begin(event);

  float val1 = _circleIn->value();
  float val2 = _bounceIn->value();
  if (_doOut)
    {
      val1 = _circleOut->value();
      val2 = _bounceOut->value();
    }

  // draw image
  p.setBrush(Color(0, 0, 0, 125 + val1 * 130));
  p.stretchImage(_image, -20 * val1, -20 * val1, width() + 40 * val1,
      height() + 40 * val1,
      (DFBSurfaceBlittingFlags) (DSBLIT_SRC_PREMULTCOLOR
          | DSBLIT_BLEND_COLORALPHA));

  // overlay rect
  int y = height() - val2 * 50;
  Color c("006666");
  c.setSaturation(val1);
  p.setBrush(c);
  p.fillRectangle(0, y, width(), 50);
  p.setBrush(Color(255, 255, 255, 255));
  p.drawLayout(_layout, 5, y);

  p.end();
}

void
ImageWidget::updateTextBaseGeometry()
{
  _layout.setBounds(0, 0, width() - 10, 50);
  _layout.doLayout(font());
}

void
ImageWidget::pointerButtonUpEvent(const PointerEvent& event)
{
  sigPressed();
}

#if POINTER_MODE
void
ImageWidget::enterEvent(const PointerEvent& event)
{
//  sigFocused(this);
  _outAni.stop();
  _doOut = false;
  _inAni.start();
}

void
ImageWidget::leaveEvent(const PointerEvent& event)
{
  _inAni.stop();
  _doOut = true;
  _outAni.start();
}
#else
void
ImageWidget::focusInEvent()
  {
    _outAni.stop();
    _doOut = false;
    _inAni.start();
  }

void
ImageWidget::focusOutEvent()
  {
    _inAni.stop();
    _doOut = true;
    _outAni.start();
  }
#endif
