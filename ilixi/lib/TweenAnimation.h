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

#ifndef ILIXI_TWEENANIMATION_H_
#define ILIXI_TWEENANIMATION_H_

#include <lib/Animation.h>
#include <lib/Tween.h>
#include <list>

namespace ilixi
{
//! Used for grouping tweens and running them.
class TweenAnimation : public Animation
{
public:
    /*!
     * Creates a new tween animation.
     */
    TweenAnimation();

    /*!
     * Deletes all tweens in list.
     */
    virtual
    ~TweenAnimation();

    /*!
     * Adds a new tween.
     */
    void
    addTween(Tween::Transition transition, Tween::Equation equation,
             float initialValue = 0, float endValue = 1);

    /*!
     * Adds the tween.
     *
     * Note that the tween is owned by this tween animation.
     */
    void
    addTween(Tween* tween);

    float
    tweenValue(int index);

protected:
    virtual void
    step(long ms);

private:
    typedef std::list<Tween*> TweenList;
    //! List of tweens.
    TweenList _tweens;

    void
    finishTweens();
};

}

#endif /* ILIXI_TWEENANIMATION_H_ */
