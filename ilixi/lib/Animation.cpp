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

#include <lib/Animation.h>
#include <core/Application.h>
#include <core/Window.h>
#include <lib/Util.h>

extern "C"
{
#include <direct/clock.h>
}

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_ANIMATION, "ilixi/lib/Animation", "Animation");

Animation::Animation()
        : _state(Stopped),
          _delayDuration(0),
          _delayTime(0),
          _delayLast(0),
          _duration(0),
          _currentTime(0),
          _lastTime(0),
          _loops(1),
          _currentLoop(1),
          _cb(this)
{
}

Animation::~Animation()
{
    stop();
}

Animation::AnimationState
Animation::state() const
{
    return _state;
}

int
Animation::duration() const
{
    return _duration;
}

int
Animation::currentTime() const
{
    return _currentTime;
}

int
Animation::delay() const
{
    return _delayDuration;
}

int
Animation::loops() const
{
    return _loops;
}

int
Animation::currentLoop() const
{
    return _currentLoop;
}

void
Animation::start()
{
    _currentLoop = 1;
    setCurrentTime();
    _state = Running;
    _cb.start();
}

void
Animation::stop()
{
    if (_state == Running)
    {
        _state = Stopped;
        _cb.stop();
    }
}

void
Animation::resume()
{
    if (_state == Paused)
    {
        setCurrentTime();
        _state = Running;
    } else if (_state == Stopped)
        start();
}

void
Animation::pause()
{
    _state = Paused;
}

void
Animation::setDelay(int ms)
{
    _delayDuration = ms;
}

void
Animation::setDuration(int ms)
{
    _duration = ms;
}

void
Animation::setLoops(int loopCount)
{
    _loops = loopCount;
}

void
Animation::setCurrentTime(int ms)
{
    if (ms)
        _currentTime = ms;
    else
        _currentTime = 0;

#if ILIXI_HAS_SURFACEEVENTS
    long long frameTime = Application::getFrameTime();
    long long actualTime = direct_clock_get_time(DIRECT_CLOCK_MONOTONIC);
    //D_INFO( "frameTime %lld, actualTime %lld (diff %lld)\n", frameTime, actualTime, actualTime - frameTime );
    _delayLast = frameTime / 1000LL;
#else
    _delayLast = direct_clock_get_abs_millis();
#endif
    _lastTime = _delayLast + _delayDuration;
}

void
Animation::setState(AnimationState state)
{
    _state = state;
}

bool
Animation::funck()
{
    if (!_duration)
    {
        _state = Stopped;
        return 0;
    } else if (_state == Running)
    {
        if (_delayTime < _delayDuration)
        {
#if ILIXI_HAS_SURFACEEVENTS
            long long frameTime = Application::getFrameTime();
            long long actualTime = direct_clock_get_time(DIRECT_CLOCK_MONOTONIC);
            //D_INFO( "frameTime %lld, actualTime %lld (diff %lld)\n", frameTime, actualTime, actualTime - frameTime );
            _delayTime = frameTime / 1000LL - _delayLast;
#else
            _delayTime = direct_clock_get_abs_millis() - _delayLast;
#endif
            return 1;
        }

        if (_currentTime <= _duration)
        {
            if (_currentTime == 0)
                sigStarted();
#if ILIXI_HAS_SURFACEEVENTS
            long long frameTime = Application::getFrameTime();
            long long actualTime = direct_clock_get_time(DIRECT_CLOCK_MONOTONIC);
            //D_INFO( "frameTime %lld, actualTime %lld (diff %lld)\n", frameTime, actualTime, actualTime - frameTime );
            long stepTime = frameTime / 1000LL - _lastTime;
#else
            long stepTime = direct_clock_get_abs_millis() - _lastTime;
#endif
            step(stepTime);
            _currentTime += stepTime;
            _lastTime += stepTime;
            sigExec();
            sigStep(stepTime);
            return 1;
        } else
            sigFinished();

        if (_loops && _currentLoop >= _loops)
        {
            _state = Stopped;
            return 0;
        } else
        {
            ++_currentLoop;
            setCurrentTime();
            return 1;
        }
    }

    return 0;
}

} /* namespace ilixi */
