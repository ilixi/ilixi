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

#include <lib/AnimationSequence.h>
#include <algorithm>
#include <sigc++/bind.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_ANIMATIONSEQ, "ilixi/lib/AnimationSequence", "AnimationSequence");

AnimationSequence::AnimationSequence()
        : _current(NULL),
          _looping(false)
{
}

AnimationSequence::~AnimationSequence()
{
    clear();
}

Animation*
AnimationSequence::currentAnimation() const
{
    return _current;
}

int
AnimationSequence::duration() const
{
    int d = 0;
    for (AnimationVector::const_iterator it = _animations.begin(); it != _animations.end(); ++it)
        d += ((Animation*) *it)->duration();
    return d;
}

bool
AnimationSequence::addAnimation(Animation* animation)
{
    if (!animation)
        return false;

    AnimationVector::iterator it = std::find(_animations.begin(), _animations.end(), animation);

    if (it != _animations.end())
    {
        ILOG_WARNING(ILX_ANIMATIONSEQ, "Animation %p is already in sequence.", animation);
        return false;
    }

    animation->sigStarted.connect(sigc::bind<Animation*>(sigc::mem_fun(this, &AnimationSequence::setCurrentAnimation), animation));

    _animations.push_back(animation);

    if (_animations.size() > 1)
        _animations[_animations.size() - 2]->sigFinished.connect(sigc::mem_fun(_animations.back(), &Animation::start));
    return true;
}

bool
AnimationSequence::removeAnimation(Animation* animation)
{
    // TODO disconnect signals.
    if (!animation)
        return false;

    AnimationVector::iterator it = std::find(_animations.begin(), _animations.end(), animation);
    if (it != _animations.end())
    {
        delete *it;
        _animations.erase(it);
        ILOG_DEBUG(ILX_ANIMATIONSEQ, "Removed animation %p\n", animation);
        return true;
    }
    return false;
}

void
AnimationSequence::clear()
{
    for (AnimationVector::iterator it = _animations.begin(); it != _animations.end(); ++it)
        delete *it;
}

void
AnimationSequence::start()
{
    if (_animations.size() && !_current)
    {
        _current = _animations[0];
        if (_looping)
            _animations.back()->sigFinished.connect(sigc::mem_fun(_animations.front(), &Animation::start));
        _current->start();
    }
}

void
AnimationSequence::stop()
{
    if (_current)
    {
        _current->stop();
        _current = NULL;
    }
}

void
AnimationSequence::setLooping(bool looping)
{
    _looping = looping;
}

void
AnimationSequence::setCurrentAnimation(Animation* animation)
{
    _current = animation;
}

} /* namespace ilixi */
