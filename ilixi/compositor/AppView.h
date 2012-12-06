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

#ifndef ILIXI_APPVIEW_H_
#define ILIXI_APPVIEW_H_

#include <compositor/AppCompositor.h>
#include <lib/TweenAnimation.h>

namespace ilixi
{
//! Helper class for animating an AppCompositor.
class AppView : public AppCompositor
{
    friend class ILXCompositor;
public:
    enum AnimatedProperty
    {
        None = 0x000,
        Zoom = 0x001,
        Opacity = 0x002,
        Position = 0x004,
        HideWhenDone = 0x008,
        AnimShowing = 0x010,
        AnimHiding = 0x020
    };

    AppView(ILXCompositor* compositor, AppInstance* instance, Widget* parent = 0);

    virtual
    ~AppView();

    void
    show(AnimatedProperty props = Opacity, int x = 0, int y = 0);

    void
    hide(AnimatedProperty props = Opacity, int x = 0, int y = 0);

    void
    setAnimatedProperty(AnimatedProperty prop);

    void
    clearAnimatedProperty(AnimatedProperty prop);

    void
    slideTo(int x, int y);

private:
    AnimatedProperty _animProps;

    TweenAnimation _propAnim;
    Tween* _opacityTween;
    Tween* _zoomTween;
    Tween* _xTween;
    Tween* _yTween;

    static int _animDuration;

    void
    tweenSlot();

    void
    tweenEndSlot();

    virtual void
    madeAvailable();
};

} /* namespace ilixi */
#endif /* ILIXI_APPVIEW_H_ */
