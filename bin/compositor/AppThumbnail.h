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

#ifndef ILIXI_APPTHUMBNAIL_H_
#define ILIXI_APPTHUMBNAIL_H_

#include "AppCompositor.h"
#include <ui/Label.h>
#include <lib/TweenAnimation.h>

namespace ilixi
{
  class Compositor;

  class AppThumbnail : public AppCompositor
  {
  public:
    AppThumbnail(Compositor* compositor, AppInstance* instance, Widget* parent =
        0);

    virtual
    ~AppThumbnail();

    Size
    preferredSize() const;

    sigc::signal<void> sigSelected;
    sigc::signal<void, AppThumbnail*> sigFocused;

  protected:
    virtual void
    compose(const PaintEvent& event);

    virtual void
    pointerButtonUpEvent(const PointerEvent& pointerEvent);

    virtual void
    keyUpEvent(const KeyEvent& keyEvent);

    virtual void
    focusInEvent();

    virtual void
    focusOutEvent();

  private:
    TweenAnimation _ani;
    Tween* _opacityTween;

    void
    tweenSlot();
  };

} /* namespace ilixi */
#endif /* ILIXI_APPTHUMBNAIL_H_ */
