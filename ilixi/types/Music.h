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

#ifndef ILIXI_MUSIC_H_
#define ILIXI_MUSIC_H_

#include <core/SoundDFB.h>
#include <string>

namespace ilixi
{

//! Plays audio streams.
/*!
 * This class is used to load and play audio streams.
 */
class Music
{
public:
    struct TrackInfo
    {
        TrackInfo()
                : artist("Unknown"),
                  title("Unknown"),
                  album("Unknown"),
                  year(0),
                  genre("Unknown"),
                  encoding("Unknown"),
                  bitrate(0),
                  replayGain(0),
                  sampleRate(0),
                  channels(0),
                  sampleFormat("Unknown")
        {
        }

        std::string artist;
        std::string title;
        std::string album;
        short year;
        std::string genre;
        std::string encoding;
        int bitrate;
        float replayGain;
        int sampleRate;
        int channels;
        std::string sampleFormat;
    };

    enum MusicState
    {
        None,
        Playing,
        Stopped,
        Finished
    };

    /*!
     * Constructor.
     */
    Music();

    /*!
     * Constructor.
     * Loads file.
     */
    Music(const std::string& filename);

    /*!
     * Destructor.
     */
    virtual
    ~Music();

    /*!
     * Returns true if provider is set to repeat.
     */
    bool
    repeat() const;

    /*!
     * Returns position in seconds.
     */
    double
    position() const;

    /*!
     * Returns volume.
     */
    float
    volume() const;

    /*!
     * Returns length of audio file in seconds.
     */
    double
    length() const;

    /*!
     * Returns trackinfo if any.
     */
    TrackInfo
    trackInfo() const;

    /*!
     * Returns status.
     */
    MusicState
    status() const;

    /*!
     * Starts playback.
     */
    void
    play();

    /*!
     * Stops playback.
     */
    void
    stop();

    /*!
     * Seeks to given position.
     */
    void
    seekTo(double position);

    /*!
     * Sets music volume.
     */
    void
    setVolume(float volume);

    /*!
     * Set whether playback loops or not.
     */
    void
    setRepeat(bool repeat);

    /*!
     * Sets path to music file.
     */
    void
    setFileName(const std::string& filename);

private:
    //! This property stores the path for audio file.
    std::string _fileName;
    //! This property stores DirectFB audio provider interface.
    IFusionSoundMusicProvider* _provider;
    //! This property stores stream.
    IFusionSoundStream* _stream;
    IFusionSoundPlayback* _playback;

    float _volume;

    bool _repeat;

    bool
    loadMusic();

    DirectEnumerationResult
    playback(FSTrackID id, FSTrackDescription desc);

    void
    release();

    friend DirectEnumerationResult
    track_display_callback(FSTrackID id, FSTrackDescription desc, void *ctx);

    friend DirectEnumerationResult
    track_playback_callback(FSTrackID id, FSTrackDescription desc, void *ctx);
};

} /* namespace ilixi */
#endif /* ILIXI_MUSIC_H_ */
