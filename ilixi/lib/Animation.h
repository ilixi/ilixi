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

#ifndef ILIXI_ANIMATION_H_
#define ILIXI_ANIMATION_H_

#include <core/Callback.h>
#include <sigc++/signal.h>

namespace ilixi
{
//! Base class for animations.
/*!
 * This class implements basic animation functionality.
 */
class Animation : public sigc::trackable, public Functionoid
{
public:

    enum AnimationState
    {
        Running,
        Paused,
        Stopped
    };

    /*!
     * Initialises a new animation.
     */
    Animation();

    /*!
     * Destructor.
     */
    virtual
    ~Animation();

    /*!
     * Returns current state of animation.
     */
    AnimationState
    state() const;

    /*!
     * Returns duration of animation in milliseconds.
     */
    int
    duration() const;

    /*!
     * Returns current animation time in milliseconds.
     */
    int
    currentTime() const;

    /*!
     * Returns animation start delay in milliseconds.
     */
    int
    delay() const;

    /*!
     * Returns number of loops.
     */
    int
    loops() const;

    /*!
     * Returns current loop number.
     */
    int
    currentLoop() const;

    /*!
     * Starts animation.
     */
    void
    start();

    /*!
     * Stops animation.
     */
    void
    stop();

    /*!
     * Resumes animation.
     */
    void
    resume();

    /*!
     * Pauses animation.
     */
    void
    pause();

    /*!
     * Sets animation start delay.
     */
    void
    setDelay(int ms);

    /*!
     * Sets duration of animation.
     *
     * @param ms duration in milliseconds.
     */
    void
    setDuration(int ms);

    /*!
     * Sets number of loops.
     *
     * @param loopCount
     */
    void
    setLoops(int loopCount);

    /*!
     * This signal is emitted when animation starts.
     */
    sigc::signal<void> sigStarted;

    /*!
     * This signal is emitted when animation is updated.
     */
    sigc::signal<void> sigExec;

    /*!
     * Same as sigExec.
     */
    sigc::signal<void, int> sigStep;

    /*!
     * This signal is emitted when animation is finished playing.
     */
    sigc::signal<void> sigFinished;

    /*!
     * This signal is emitted when animation finishes a loop.
     */
    sigc::signal<int> sigLoopChanged;

    //! Sets current time of animation.
    void
    setCurrentTime(int ms = 0);

protected:
    //! Sets animation state.
    virtual void
    setState(AnimationState state);

    //! Callback which iterates animation.
    bool
    funck();

    //! Reimplement this method...
    virtual void
    step(long ms)=0;

private:
    //! Current state of animation.
    AnimationState _state;

    //! Delay before animation starts.
    int _delayDuration;
    //! Current delay timer.
    int _delayTime;
    //! Measured time for delay.
    long _delayLast;

    //! Duration of animation in milliseconds.
    int _duration;
    //! Current animation time in milliseconds.
    int _currentTime;
    //! Last measured time in milliseconds.
    long _lastTime;

    //! Loop count of animation.
    int _loops;
    //! Current loop in animation.
    int _currentLoop;
    //! Callback for animation.
    Callback _cb;

    // Callback uses animate()
    friend bool
    animationCallback(void*);
};

}

#endif /* ILIXI_ANIMATION_H_ */
