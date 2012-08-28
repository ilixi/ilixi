/*
 * Music.h
 *
 *  Created on: Aug 28, 2012
 *      Author: tarik
 */

#ifndef ILIXI_MUSIC_H_
#define ILIXI_MUSIC_H_

#include <core/SoundDFB.h>
#include <string>

namespace ilixi
{

class Music
{
public:
    struct TrackInfo
    {
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

    Music(const std::string& filename);

    virtual
    ~Music();

    bool
    repeat() const;

    double
    position() const;

    float
    volume() const;

    double
    length() const;

    TrackInfo
    trackInfo() const;

    MusicState
    status() const;

    void
    play();

    void
    stop();

    void
    seekTo(double position);

    void
    setVolume(float volume);

    void
    setRepeat(bool repeat);

private:
    std::string _fileName;
    IFusionSoundMusicProvider* _provider;
    IFusionSoundStream* _stream;
    IFusionSoundPlayback* _playback;
    float _volume;
    bool _repeat;

    void
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
