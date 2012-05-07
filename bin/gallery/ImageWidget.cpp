/*
 * ImageWidget.cpp
 *
 *  Created on: 10 Nov 2011
 *      Author: tarik
 */

#include "ImageWidget.h"
#include "graphics/Painter.h"
#include "core/Logger.h"

using namespace ilixi;

ImageWidget::ImageWidget(const std::string& text, Widget* parent) :
    Button(text, parent), _doOut(false)
{
  _inAni.setDuration(500);
  _inAni.sigExec.connect(sigc::mem_fun(this, &ImageWidget::tweenSlot));
  _circleIn = new Tween(Tween::CIRCLE, Tween::EASE_OUT, 0, 1);
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
ImageWidget::compose(const Rectangle& rect)
{
  Painter p(this);
  p.begin(rect);

  float val1 = _circleIn->value();
  float val2 = _bounceIn->value();
  if (_doOut)
    {
      val1 = _circleOut->value();
      val2 = _bounceOut->value();
    }

  // draw image
  p.setBrush(Color(0, 0, 0, 125 + val1 * 130));
  p.drawImage(_image, -20 * val1, -20 * val1, width() + 40 * val1,
      height() + 40 * val1,
      (DFBSurfaceBlittingFlags) (DSBLIT_SRC_PREMULTCOLOR | DSBLIT_BLEND_COLORALPHA));

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
ImageWidget::updateTextLayoutGeometry()
{
  _layout.setBounds(0, 0, width() - 10, 50);
  _layout.doLayout(font());
}

void
ImageWidget::pointerButtonUpEvent(const PointerEvent& event)
{
  sigPressed(_layout.text());
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
