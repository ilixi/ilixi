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

#include <lib/Timer.h>
#include <core/AppBase.h>
#include <core/Window.h>
#include <core/Logger.h>

extern "C"
{
#include <direct/clock.h>
}

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_TIMER, "ilixi/lib/Timer", "Timer");

Timer::Timer()
        : _interval(500),
          _repeats(0),
          _count(0),
          _running(false),
          _expiry(0)
{
}

Timer::~Timer()
{
    AppBase::removeTimer(this);
}

bool
Timer::running() const
{
    return _running;
}

unsigned int
Timer::interval() const
{
    return _interval;
}

unsigned int
Timer::repeats() const
{
    return _repeats;
}

void
Timer::start(unsigned int msec, unsigned int repeats)
{
    _running = true;
    _interval = msec;
    _count = 0;
    _repeats = repeats;
    _expiry = direct_clock_get_millis() + _interval;
    ILOG_TRACE_F(ILX_TIMER);
    ILOG_DEBUG(
            ILX_TIMER,
            " -> Interval %d msec (trigger time %d.%d)\n", _interval, (int) (_expiry/1000), (int)(_expiry%1000));
    AppBase::addTimer(this);
}

void
Timer::stop()
{
    ILOG_TRACE_F(ILX_TIMER);
    _running = false;
    AppBase::removeTimer(this);
}

void
Timer::setInterval(unsigned int msec)
{
    _interval = msec;
}

void
Timer::setRepeats(unsigned int number)
{
    _repeats = number;
}

void
Timer::notify()
{
}

int64_t
Timer::expiry() const
{
    return _expiry;
}

bool
Timer::funck()
{
    if (_repeats && _repeats == _count)
    {
        _running = false;
        return false;
    }
    _expiry = direct_clock_get_millis() + _interval;

    ILOG_DEBUG(
            ILX_TIMER,
            "Timer[%p] next timeout %d.%d\n", this, (int) (_expiry/1000), (int)(_expiry%1000));

    ++_count;
    notify();
    sigExec();
    return true;
}

} /* namespace ilixi */
