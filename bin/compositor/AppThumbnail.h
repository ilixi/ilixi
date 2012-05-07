/*
 Copyright 2012 Tarik Sekmen.

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
#include "ui/Label.h"
#include "lib/TweenAnimation.h"

namespace ilixi
{

  class AppThumbnail : public AppCompositor
  {
  public:
    AppThumbnail(const std::string& text, AppFlags flags = APP_NONE,
        Widget* parent = 0);

    virtual
    ~AppThumbnail();

    Size
    preferredSize() const;

    void
    showOverlay();

    void
    hideOverlay();

    sigc::signal<void> sigSelected;
    sigc::signal<void, AppThumbnail*> sigFocused;

  protected:
    virtual void
    compose(const Rectangle& rect);

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
    Tween* _overlayTween;
    Tween* _opacityTween;

    Label* _label;

    void
    tweenSlot();

    void
    onAppThumbnailGeometryUpdated();
  };

} /* namespace ilixi */
#endif /* ILIXI_APPTHUMBNAIL_H_ */
