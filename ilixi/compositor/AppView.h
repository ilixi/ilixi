/*
 Copyright 2010-2013 Tarik Sekmen.

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
//! Provides a view for an application instance.
class AppView : public AppCompositor
{
    friend class ILXCompositor;
public:
    /*!
     * This enum specifies properties which are animated while showing, hiding view.
     */
    enum AnimatedProperty
    {
        None = 0x000,        //!< None
        Zoom = 0x001,        //!< Zoom
        Opacity = 0x002,     //!< Opacity
        Position = 0x004,    //!< Position
        HideWhenDone = 0x008,//!< HideWhenDone
        AnimShowing = 0x010, //!< AnimShowing
        AnimHiding = 0x020   //!< AnimHiding
    };

    /*!
     * Constructor.
     */
    AppView(ILXCompositor* compositor, AppInstance* instance, Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~AppView();

    /*!
     * Shows application view.
     *
     * @param props ORed properties which will be animated.
     * @param x target x coordinate.
     * @param y target y coordinate.
     */
    void
    show(AnimatedProperty props = Opacity, int x = 0, int y = 0);

    /*!
     * Hides application view.
     *
     * @param props ORed properties which will be animated.
     * @param x target x coordinate.
     * @param y target y coordinate.
     */
    void
    hide(AnimatedProperty props = Opacity, int x = 0, int y = 0);

    /*!
     * Moves application view to given coordinates.
     *
     * @param x target x coordinate.
     * @param y target y coordinate.
     */
    void
    slideTo(int x, int y);

protected:
    //! Sets given flag.
    void
    setAnimatedProperty(AnimatedProperty prop);

    //! Clear given flag.
    void
    clearAnimatedProperty(AnimatedProperty prop);

private:
    //! This property stores ORed animated properties.
    AnimatedProperty _animProps;
    //! This is the property animation. Sets duration.
    TweenAnimation _propAnim;
    //! Tween for modifying opacity.
    Tween* _opacityTween;
    //! Tween for modifying zoom factor.
    Tween* _zoomTween;
    //! Tween for modifying x coordinate.
    Tween* _xTween;
    //! Tween for modifying y coordinate.
    Tween* _yTween;

    //! Updates with new tween values.
    void
    tweenSlot();

    //! Clears flags.
    void
    tweenEndSlot();

    //! Overloaded. Specifies behaviour using AppFlags.
    virtual void
    madeAvailable();
};

} /* namespace ilixi */
#endif /* ILIXI_APPVIEW_H_ */
