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
    Button(text, parent), _val1(0), _valBounce(0), _iconT(0)
{
  _inAni = new TweenAnimation();
  _inAni->addTween(Tween::CIRCLE, Tween::EASE_OUT, _val1, 0, 1);
  _inAni->addTween(Tween::BOUNCE, Tween::EASE_OUT, _valBounce, 0, 1);
  _inAni->setDuration(500);
  _inAni->sigExec.connect(sigc::mem_fun(this, &ImageWidget::tweenSlot));

  _outAni = new TweenAnimation();
  _outAni->addTween(Tween::SINE, Tween::EASE_OUT, _val1, 1, 0);
  _outAni->addTween(Tween::SINE, Tween::EASE_OUT, _valBounce, 1, 0);
  _outAni->setDuration(300);
  _outAni->sigExec.connect(sigc::mem_fun(this, &ImageWidget::tweenSlot));

  setInputMethod(KeyAndPointerInput);
}

ImageWidget::~ImageWidget()
{
  delete _image;
  delete _inAni;
  delete _outAni;
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
ImageWidget::compose()
{
  Painter p(this);
  p.begin();

  // draw image
  p.setBrush(Color(0, 0, 0, 125 + _val1 * 130));
  p.drawImage(_image, -20 * _val1, -20 * _val1, width() + 40 * _val1,
      height() + 40 * _val1,
      (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_COLORALPHA));

  // overlay rect
  int y = height() - _valBounce * 50;
  Color c("006666");
  c.setSaturation(_val1);
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
  _outAni->stop();
  _inAni->start();
}

void
ImageWidget::leaveEvent(const PointerEvent& event)
{
  _inAni->stop();
  _outAni->start();
}
#else
void
ImageWidget::focusInEvent()
  {
    _outAni->stop();
    _inAni->start();
  }

void
ImageWidget::focusOutEvent()
  {
    _inAni->stop();
    _outAni->start();
  }
#endif
