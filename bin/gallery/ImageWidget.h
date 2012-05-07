/*
 * ImageWidget.h
 *
 *  Created on: 10 Nov 2011
 *      Author: tarik
 */

#ifndef IMAGEWIDGET_H_
#define IMAGEWIDGET_H_

#include "ui/Button.h"
#include "lib/TweenAnimation.h"

#define POINTER_MODE true

class ImageWidget : public ilixi::Button
{
public:
  ImageWidget(const std::string& text, ilixi::Widget* parent = 0);

  ~ImageWidget();

  void
  setImage(ilixi::Image* image);

  sigc::signal<void, std::string> sigPressed;

protected:
  void
  tweenSlot();

  virtual void
  compose();

  virtual void
  pointerButtonUpEvent(const ilixi::PointerEvent& event);

  void
  updateTextLayoutGeometry();

#if POINTER_MODE
  /*!
   * Starts/stops tweens.
   */
  virtual void
  enterEvent(const ilixi::PointerEvent& event);

  /*!
   * Starts/stops tweens.
   */
  virtual void
  leaveEvent(const ilixi::PointerEvent& event);
#else
  /*!
   * Starts/stops tweens.
   */
  virtual void
  focusInEvent();

  /*!
   * Starts/stops tweens.
   */
  virtual void
  focusOutEvent();
#endif

private:
  bool _doOut;
  ilixi::Image* _image;
  ilixi::TweenAnimation _inAni;
  ilixi::Tween* _circleIn;
  ilixi::Tween* _bounceIn;

  ilixi::TweenAnimation _outAni;
  ilixi::Tween* _circleOut;
  ilixi::Tween* _bounceOut;
};

#endif /* IMAGEWIDGET_H_ */
