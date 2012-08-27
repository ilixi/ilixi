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

#ifndef ILIXI_SOUND_H_
#define ILIXI_SOUND_H_

#include <directfb.h>
extern "C"
{
#include <fusionsound.h>
}

namespace ilixi
{

class SoundDFB
{
    friend class AppBase;
public:
    static void
    getMasterAmplitude(float* left, float* right);

    static DFBResult
    createBuffer(const FSBufferDescription* desc, IFusionSoundBuffer** buffer);

    static DFBResult
    createStream(const FSStreamDescription* desc, IFusionSoundStream** stream);

    static DFBResult
    createMusicProvider(const char* filename,
                        IFusionSoundMusicProvider** provider);

private:
    //! Main FusionSound interface.
    static IFusionSound* __sound;

    SoundDFB();

    virtual
    ~SoundDFB();

    static DFBResult
    initSound(int* argc, char*** argv);

    static void
    releaseSound();
};

} /* namespace ilixi */
#endif /* ILIXI_SOUND_H_ */
