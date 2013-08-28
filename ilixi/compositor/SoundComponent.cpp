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

#include <compositor/SoundComponent.h>
#include <core/SoundDFB.h>
#include <core/ComponentData.h>
#include <core/PlatformManager.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_SOUNDCOMP, "ilixi/Coma/SoundMixer", "SoundComponent");

SoundComponent::SoundComponent()
        : ComaComponent("SoundMixer", SoundMixer::SMNumNotifications),
          _volume(1)
{
    init();
    createNotification(SoundMixer::VolumeChanged, NULL);
    SoundDFB::setMasterVolume(1);
}

SoundComponent::~SoundComponent()
{
}

DirectResult
SoundComponent::comaMethod(ComaMethodID method, void *arg)
{
    ILOG_TRACE_F(ILX_SOUNDCOMP);
    DirectResult ret = DR_OK;
    switch (method)
    {
    case SoundMixer::SetVolume:
        {
            float volume = *((float*) arg);
            _volume = volume;
            ILOG_DEBUG(ILX_SOUNDCOMP, " -> SetVolume ( %f )\n", volume);
            setVolume(volume);
        }
        break;

    case SoundMixer::ToggleMute:
        ILOG_DEBUG(ILX_SOUNDCOMP, " -> ToggleMute: %f\n", _volume);
        if (SoundDFB::getMasterVolume() == 0)
            setVolume(_volume);
        else
            setVolume(0);
        break;

    case SoundMixer::PlaySoundEffect:
        {
            const char* id = ((const char*) arg);
            ILOG_DEBUG(ILX_SOUNDCOMP, " -> PlaySoundEffect ( %s )\n", id);
            PlatformManager::instance().playSoundEffect(id);
        }
        break;

    case SoundMixer::SetSoundEffectVolume:
        {
            float level = *((float*) arg);
            ILOG_DEBUG(ILX_SOUNDCOMP, " -> SetSoundEffectVolume ( %f )\n", level);
            PlatformManager::instance().setSoundEffectLevel(level);
        }
        break;

    default:
        ret = DR_NOIMPL;
        break;
    }
    return ret;
}

void
SoundComponent::setVolume(float volume)
{
    ILOG_TRACE_F(ILX_SOUNDCOMP);
    ILOG_DEBUG(ILX_SOUNDCOMP, " -> volume: %f\n", volume);

    SoundDFB::setMasterVolume(volume);

    float* vol;
    allocate(sizeof(vol), (void**) &vol);
    *vol = volume;
    notify(0, vol);
}

} /* namespace ilixi */
