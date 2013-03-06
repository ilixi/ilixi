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

#ifndef ILIXI_SOUND_H_
#define ILIXI_SOUND_H_

#include <core/SoundDFB.h>
#include <string>

namespace ilixi
{
//! Plays raw and uncompressed static sound buffers.
/*!
 * You should use this class if you want to play short sound effects for
 * user interface events.
 *
 * @warning Only WAVE is supported.
 */
class Sound
{
public:
    //! This enum specifies direction of playback.
    enum PlaybackDirection
    {
        Forward = 1,    //!< Forward, default.
        Backward = -1   //!< Backward.
    };

    /*!
     * Constructor.
     */
    Sound();

    /*!
     * Constructor sets filename.
     */
    Sound(const std::string& filename);

    /*!
     * Destructor.
     */
    virtual
    ~Sound();

    /*!
     * Starts playback of the buffer.
     *
     * @param looping if true will play buffer in looping mode.
     */
    void
    start(bool looping = false);

    /*!
     * Stops playback of the buffer.
     */
    void
    stop();

    /*!
     * Sets volume level [0.0, 1.0].
     */
    void
    setVolume(float level);

    /*!
     * Sets panning value [-1.0: left, 1.0: right].
     */
    void
    setPan(float pan);

    /*!
     * Sets pitch value [0.0, 1.0].
     */
    void
    setPitch(float value);

    /*!
     * Sets playback direction.
     */
    void
    setDirection(PlaybackDirection direction);

    /*!
     * Sets path to audio file.
     */
    void
    setFileName(const std::string& filename);

private:
    //! This property stores the path for audio file.
    std::string _fileName;
    //! This property stores static sound buffer.
    IFusionSoundBuffer* _buffer;
    //! This property stores playback interface.
    IFusionSoundPlayback* _playback;

    //! Loads an audio file to a FusionSoundBuffer.
    void
    loadSample();

    //! Releases FusionSound interfaces.
    void
    release();
};

} /* namespace ilixi */
#endif /* ILIXI_SOUND_H_ */
