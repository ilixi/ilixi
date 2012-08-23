/*
 * Sound.cpp
 *
 *  Created on: Aug 23, 2012
 *      Author: tarik
 */

#include "Sound.h"
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_SOUND, "ilixi/lib/Sound", "Sound");

Sound::Sound()
{
    static bool init = false;
    if (!init)
    {
        if (FusionSoundInit(NULL, NULL) != DR_OK)
            ILOG_THROW(ILX_SOUND, "FusionSoundInit\n");
        init = true;
    }

    if (FusionSoundCreate(&_sound) != DR_OK)
        ILOG_THROW(ILX_SOUND, "FusionSoundCreate\n");
}

Sound::~Sound()
{
    _sound->Release(_sound);
}

void
Sound::getMasterAmplitude(float* left, float* right)
{
    _sound->GetMasterFeedback(_sound, left, right);
}

} /* namespace ilixi */
