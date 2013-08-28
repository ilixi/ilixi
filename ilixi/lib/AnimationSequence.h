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

#ifndef ILIXI_ANIMATIONSEQUENCE_H_
#define ILIXI_ANIMATIONSEQUENCE_H_

#include <lib/Animation.h>
#include <vector>

namespace ilixi
{
//! Allows to run animations in sequential order.
class AnimationSequence
{
public:
    /*!
     * Constructor.
     */
    AnimationSequence();

    /*!
     * Destructor.
     */
    virtual
    ~AnimationSequence();

    /*!
     * Returns currently running animation, if any.
     */
    Animation*
    currentAnimation() const;

    /*!
     * Returns the total duration of sequence.
     */
    int
    duration() const;

    /*!
     * Adds animation to the end of sequence.
     */
    bool
    addAnimation(Animation* animation);

    /*!
     * Removes animation from sequence.
     */
    bool
    removeAnimation(Animation* animation);

    /*!
     * Removes all animations.
     */
    void
    clear();

    /*!
     * Starts running sequence.
     */
    void
    start();

    /*!
     * Stops sequence.
     */
    void
    stop();

    /*!
     * Sets whether sequence loops.
     */
    void
    setLooping(bool looping);

private:
    //! This property stores the currently running animation.
    Animation* _current;
    bool _looping;
    typedef std::vector<Animation*> AnimationVector;
    AnimationVector _animations;

    //! Sets current animation, used by signals.
    void
    setCurrentAnimation(Animation* animation);
};

} /* namespace ilixi */
#endif /* ILIXI_ANIMATIONSEQUENCE_H_ */
