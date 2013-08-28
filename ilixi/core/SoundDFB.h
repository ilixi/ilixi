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

#ifndef ILIXI_SOUNDDFB_H_
#define ILIXI_SOUNDDFB_H_

#include <ilixiConfig.h>
#include <directfb.h>

#ifdef ILIXI_HAVE_FUSIONSOUND
extern "C"
{
#include <fusionsound.h>
}
#else
typedef void FSBufferDescription;
typedef void FSStreamDescription;
typedef void IFusionSound;
typedef void IFusionSoundBuffer;
typedef void IFusionSoundStream;
typedef void IFusionSoundMusicProvider;
#endif

namespace ilixi
{

//! Provides access to FusionSound interfaces.
class SoundDFB
{
    friend class PlatformManager;
public:
    /*!
     * Returns master output volume.
     */
    static float
    getMasterVolume();

    /*!
     * Sets master output volume.
     */
    static void
    setMasterVolume(float volume);

    /*!
     * Returns amplitude levels.
     */
    static void
    getMasterAmplitude(float* left, float* right);

    /*!
     * Creates a static sound buffer.
     */
    static DFBResult
    createBuffer(const FSBufferDescription* desc, IFusionSoundBuffer** buffer);

    /*!
     * Creates a streaming sound buffer.
     */
    static DFBResult
    createStream(const FSStreamDescription* desc, IFusionSoundStream** stream);

    /*!
     * Creates a music provider.
     */
    static DFBResult
    createMusicProvider(const char* filename, IFusionSoundMusicProvider** provider);

private:
    //! Main FusionSound interface.
    static IFusionSound* __sound;

    /*!
     * Constructor.
     */
    SoundDFB();

    /*!
     * Destructor.
     */
    virtual
    ~SoundDFB();

    /*!
     * Initialises FusionSound interfaces.
     */
    static DFBResult
    initSound(int* argc, char*** argv);

    /*!
     * Releases FusionSound interfaces.
     */
    static void
    releaseSound();
};

} /* namespace ilixi */
#endif /* ILIXI_SOUNDDFB_H_ */
