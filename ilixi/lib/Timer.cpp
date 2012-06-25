/*
 Copyright 2011 Tarik Sekmen.

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

#include "lib/Timer.h"
#include "core/Window.h"
#include <direct/clock.h>

namespace ilixi
{

  Timer::Timer() :
      _interval(500), _repeats(0), _count(0), _lastTime(0), _cb(this)
  {
  }

  Timer::~Timer()
  {
  }

  int
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
  Timer::start(int msec)
  {
    _interval = msec;
    _count = 0;
    _cb.start();
    _lastTime = direct_clock_get_abs_millis();
  }

  void
  Timer::stop()
  {
    _cb.stop();
  }

  void
  Timer::setInterval(int msec)
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

  bool
  Timer::funck()
  {
    if (_repeats && _repeats == _count)
      return false; // TODO should remove callback

    long long stepTime = direct_clock_get_abs_millis() - _lastTime;
    if (stepTime >= _interval)
      {
        _lastTime += stepTime;
        ++_count;
        notify();
        sigExec();
        return true;
      }
    return false;
  }

} /* namespace ilixi */
