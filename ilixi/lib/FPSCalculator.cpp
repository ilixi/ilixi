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

#include <lib/FPSCalculator.h>
#include <directfb.h>
extern "C"
{
#include <direct/clock.h>
}

namespace ilixi
{

FPSCalculator::FPSCalculator()
        : _frames(0),
          _fps(0),
          _fpsTime(direct_clock_get_millis())
{
}

FPSCalculator::~FPSCalculator()
{
}

float
FPSCalculator::fps() const
{
    return _fps;
}

bool
FPSCalculator::funck()
{
    long long diff;
    long long now = direct_clock_get_millis();

    _frames++;

    diff = now - _fpsTime;
    if (diff >= 1000)
    {
        _fps = _frames * 1000 / (float) (diff);
        _fpsTime = now;
        _frames = 0;
        sigUpdated(_fps);
    }
    return true;
}

} /* namespace ilixi */
