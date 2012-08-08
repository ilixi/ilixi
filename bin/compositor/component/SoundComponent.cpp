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

#include "SoundComponent.h"
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_SOUNDCOMP, "ilixi/Coma/SoundComponent", "SoundComponent");

SoundComponent::SoundComponent()
        : ComaComponent("SoundComponent", SoundCompNumNotifications),
          _volume(50)
{
    init();
    createNotification(0, NULL);
    createNotification(1, NULL);
    createNotification(2, NULL);
}

SoundComponent::~SoundComponent()
{
}

DirectResult
SoundComponent::comaMethod(ComaMethodID method, void *arg)
{
    DirectResult ret = DR_OK;
    int volume = *((int*) arg);
    switch (method)
    {
    case SetVolume:
        ILOG_DEBUG(ILX_SOUNDCOMP, "SetVolume\n");
        setVolume(volume);
        break;

    case Mute:
        ILOG_DEBUG(ILX_SOUNDCOMP, "Mute\n");
        setVolume(0);
        break;

    case UnMute:
        ILOG_DEBUG(ILX_SOUNDCOMP, "UnMute\n");
        setVolume(50);
        break;

    default:
        ret = DR_NOIMPL;
        break;
    }
    return ret;
}

void
SoundComponent::setVolume(unsigned int volume)
{
    _volume = volume;
    int* vol;
    allocate(sizeof(vol), (void**) &vol);
    *vol = volume;
    notify(0, vol);
    ILOG_DEBUG(ILX_SOUNDCOMP, "Volume is %d\n", _volume);
}

} /* namespace ilixi */
