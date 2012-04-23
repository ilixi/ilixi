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

#include "core/Callback.h"
#include "core/AppBase.h"
#include "lib/Animation.h"
#include "lib/Timer.h"

namespace ilixi
{
  bool
  defaultCallback(void* data)
  {
    return 1;
  }

  bool
  animationCallback(void* animation)
  {
    return ((Animation*) animation)->animate();
  }

  bool
  timerCallback(void* timer)
  {
    return ((Timer*) timer)->step();
  }

  Callback::Callback(CallbackType type, void* data) :
      _func(NULL), _data(data)
  {
    switch (type)
      {
    case AnimationCB:
      _func = animationCallback;
      break;
    case TimerCB:
      _func = timerCallback;
      break;
    default:
      _func = defaultCallback;
      break;
      }
  }

  Callback::~Callback()
  {
    stop();
  }

  void
  Callback::start()
  {
    AppBase::addCallback(this);
  }

  void
  Callback::stop()
  {
    AppBase::removeCallback(this);
  }
}
