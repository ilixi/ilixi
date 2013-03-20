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

#ifndef ILIXI_VKMENU_H_
#define ILIXI_VKMENU_H_

#include "MenuItem.h"
#include "ui/ScrollArea.h"
#include "lib/TweenAnimation.h"
#include "ImageDialog.h"

namespace ilixi
{

  class VKMenu : public ilixi::Widget
  {
  public:
    VKMenu(Widget* parent = 0);

    virtual
    ~VKMenu();

    virtual ilixi::Size
    preferredSize() const;

    void
    showScroller(MenuItem* widget);

    void
    clearScrollerAnimDelay();

    void
    scrollTo(Widget* widget);

    void
    tweenSlot();

//    virtual void
//    paint(const PaintEvent& event);

  protected:
    ScrollArea* _level1;

    TweenAnimation _inAni;
    Tween* _bounce;
    Tween* _circle;

    ImageDialog* _dialog;

    virtual void
    compose(const PaintEvent& event);

    void
    updateVKMenuGeometry();
  };

} /* namespace ilixi */
#endif /* ILIXI_VKMENU_H_ */
