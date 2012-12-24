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

#include <compositor/AppCompositor.h>
#include <lib/TweenAnimation.h>
#include <ui/ToolButton.h>

namespace ilixi
{
class ILXCompositor;

class AppThumbnail : public AppCompositor
{
public:
    AppThumbnail(ILXCompositor* compositor, AppInstance* instance, Widget* parent = 0);

    virtual
    ~AppThumbnail();

    Size
    preferredSize() const;

    void
    setCloseVisible(bool visible);

    sigc::signal<void, AppInstance*> sigSelected;
    sigc::signal<void, AppThumbnail*> sigFocused;
    ToolButton* _close;

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

    void
    updateThumbGeometry();
};

} /* namespace ilixi */
#endif /* ILIXI_APPTHUMBNAIL_H_ */
