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

#ifndef SWITCHER_H_
#define SWITCHER_H_

#include "ui/ScrollArea.h"
#include "ui/HBoxLayout.h"
#include "AppThumbnail.h"

namespace ilixi
{

  class Switcher : public Widget
  {
  public:
    Switcher(Widget* parent = 0);

    virtual
    ~Switcher();

    virtual Size
    preferredSize() const;

    void
    addThumb(AppThumbnail* thumb);

    void
    removeThumb(AppThumbnail* thumb);

    void
    scrollTo(AppThumbnail* thumb);

    void
    scrollToNext();

    void
    scrollToPrevious();

    AppThumbnail*
    currentThumb();

    AppThumbnail*
    nextThumb();

    AppThumbnail*
    previousThumb();

    void
    show();

    void
    hide();

    sigc::signal<void> sigSwitchRequest;

  protected:
    AppThumbnail* _current;

    unsigned int _currentID;

    typedef std::vector<AppThumbnail*> Thumbnails;
    Thumbnails _thumbs;

    ScrollArea* _scrollArea;
    HBoxLayout* _box;

    virtual void
    compose(const Rectangle& rect);

    void
    updateSwitcherGeometry();

  private:
    TweenAnimation _anim;
    Tween* _tween;

    void
    tweenSlot();

    void
    tweenEndSlot();

    void
    viewRequest();
  };

} /* namespace ilixi */
#endif /* SWITCHER_H_ */
