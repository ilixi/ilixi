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

#include <core/SoundDFB.h>
#include <core/Logger.h>

namespace ilixi
{

IFusionSound* SoundDFB::__sound = NULL;

D_DEBUG_DOMAIN( ILX_SOUND, "ilixi/core/Sound", "Sound");

SoundDFB::SoundDFB()
{
}

SoundDFB::~SoundDFB()
{
}

void
SoundDFB::setMasterVolume(float volume)
{
    ILOG_TRACE_F(ILX_SOUND);
    if (__sound)
        __sound->SetMasterVolume(volume);
}

void
SoundDFB::getMasterAmplitude(float* left, float* right)
{
    ILOG_TRACE_F(ILX_SOUND);
    if (__sound)
        __sound->GetMasterFeedback(__sound, left, right);
}

DFBResult
SoundDFB::createBuffer(const FSBufferDescription* desc, IFusionSoundBuffer** buffer)
{
    ILOG_TRACE_F(ILX_SOUND);
    if (!__sound)
        return DFB_FAILURE;

    DirectResult ret = __sound->CreateBuffer(__sound, desc, buffer);
    if (ret)
    {
        ILOG_ERROR(ILX_SOUND, "CreateBuffer() failed!");
        return DFB_FAILURE;
    }

    return DFB_OK;
}

DFBResult
SoundDFB::createStream(const FSStreamDescription* desc, IFusionSoundStream** stream)
{
    ILOG_TRACE_F(ILX_SOUND);
    if (!__sound)
        return DFB_FAILURE;

    DirectResult ret = __sound->CreateStream(__sound, desc, stream);
    if (ret)
    {
        ILOG_ERROR(ILX_SOUND, "CreateMusicProvider() failed!");
        return DFB_FAILURE;
    }

    return DFB_OK;
}

DFBResult
SoundDFB::createMusicProvider(const char* filename, IFusionSoundMusicProvider** provider)
{
    ILOG_TRACE_F(ILX_SOUND);
    if (!__sound)
        return DFB_FAILURE;

    DirectResult ret = __sound->CreateMusicProvider(__sound, filename,
                                                    provider);
    if (ret)
    {
        ILOG_ERROR(ILX_SOUND, "CreateMusicProvider() failed!");
        return DFB_FAILURE;
    }

    return DFB_OK;
}

DFBResult
SoundDFB::initSound(int* argc, char*** argv)
{
    ILOG_TRACE_F(ILX_SOUND);
    if (FusionSoundInit(argc, argv) != DR_OK)
        ILOG_THROW(ILX_SOUND, "FusionSoundInit() failed!\n");

    if (FusionSoundCreate(&__sound) != DR_OK)
        ILOG_THROW(ILX_SOUND, "FusionSoundCreate() failed!\n");
    return DFB_OK;
}

void
SoundDFB::releaseSound()
{
    ILOG_TRACE_F(ILX_SOUND);
    if (__sound)
    {
        __sound->Release(__sound);
        __sound = NULL;
    }
}

} /* namespace ilixi */
