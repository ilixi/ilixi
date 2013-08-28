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

#include <lib/TweenAnimation.h>
#include <algorithm>
#include <stdio.h>

namespace ilixi
{

TweenAnimation::TweenAnimation()
        : Animation()
{
    // FIXME Sets to end value of tween
    sigFinished.connect(sigc::mem_fun(this, &TweenAnimation::finishTweens));
}

TweenAnimation::~TweenAnimation()
{
    for (TweenList::iterator it = _tweens.begin(); it != _tweens.end(); ++it)
        delete *it;
}

void
TweenAnimation::addTween(Tween::Transition transition, Tween::Equation equation, float initialValue, float endValue)
{
    Tween* t = new Tween(transition, equation, initialValue, endValue);
    _tweens.push_back(t);
}

void
TweenAnimation::addTween(Tween* tween)
{
    _tweens.push_back(tween);
}

float
TweenAnimation::tweenValue(int index)
{
    int i = 0;
    for (TweenList::iterator it = _tweens.begin(); it != _tweens.end(); ++it, ++i)
        if (i == index)
            return ((Tween*) *it)->value();
    return 0;
}

void
TweenAnimation::step(long ms)
{
    int t = currentTime();
    int d = duration();
    for (TweenList::iterator it = _tweens.begin(); it != _tweens.end(); ++it)
        ((Tween*) *it)->runEase(t, d);
}

void
TweenAnimation::finishTweens()
{
    for (TweenList::iterator it = _tweens.begin(); it != _tweens.end(); ++it)
        ((Tween*) *it)->_value = ((Tween*) *it)->endValue();
    sigExec();
}

} /* namespace ilixi */
