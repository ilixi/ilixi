/*
 * HomeButton.h
 *
 *  Created on: Apr 3, 2012
 *      Author: tarik
 */

#ifndef HOMEBUTTON_H_
#define HOMEBUTTON_H_

#include "ui/Widget.h"
#include "types/Image.h"
#include "lib/TweenAnimation.h"

namespace ilixi
{

  class HomeButton : public Widget
  {
  public:
    HomeButton(Widget* parent = 0);

    virtual
    ~HomeButton();

    virtual Size
    preferredSize() const;

    void
    show();

    void
    hide();

    sigc::signal<void> sigPressed;

  protected:
    bool _click;
    TweenAnimation _anim;
    Tween* _tween;

    virtual void
    pointerButtonDownEvent(const PointerEvent& pointerEvent);

    virtual void
    pointerButtonUpEvent(const PointerEvent& pointerEvent);

    void
    compose();

  private:
    Image* _image;

    void
    tweenSlot();

    void
    tweenEndSlot();

    void
    updateHBGeometry();
  };

} /* namespace ilixi */
#endif /* HOMEBUTTON_H_ */
