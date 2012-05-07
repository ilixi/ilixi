/*
 * SwitchButton.h
 *
 *  Created on: Apr 3, 2012
 *      Author: tarik
 */

#ifndef SWITCHBUTTON_H_
#define SWITCHBUTTON_H_

#include "ui/Widget.h"
#include "types/Image.h"
#include "lib/TweenAnimation.h"

namespace ilixi
{

  class SwitchButton : public Widget
  {
  public:
    SwitchButton(Widget* parent = 0);

    virtual
    ~SwitchButton();

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
    updateSBGeometry();
  };

} /* namespace ilixi */
#endif /* SWITCHBUTTON_H_ */
