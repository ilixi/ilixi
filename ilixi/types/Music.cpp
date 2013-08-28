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

#include <types/Music.h>
#include <lib/FileSystem.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_MUSIC, "ilixi/types/Music", "Music");

DirectEnumerationResult
track_display_callback(FSTrackID id, FSTrackDescription desc, void *ctx)
{
    printf("  Track %2d: %s - %s\n", id, *desc.artist ? desc.artist : "Unknown", *desc.title ? desc.title : "Unknown");

    return DENUM_OK;
}

DirectEnumerationResult
track_playback_callback(FSTrackID id, FSTrackDescription desc, void *ctx)
{
    Music* music = (Music*) ctx;

    return music->playback(id, desc);
}

Music::Music()
        : _fileName(""),
          _provider(NULL),
          _stream(NULL),
          _playback(_playback),
          _volume(1.0),
          _repeat(false)
{
    ILOG_TRACE_F(ILX_MUSIC);
}

Music::Music(const std::string& fileName)
        : _fileName(fileName),
          _provider(NULL),
          _stream(NULL),
          _playback(_playback),
          _volume(1.0),
          _repeat(false)
{
    ILOG_TRACE_F(ILX_MUSIC);
    loadMusic();
}

Music::~Music()
{
    ILOG_TRACE_F(ILX_MUSIC);
    release();
}

bool
Music::repeat() const
{
    return _repeat;
}

double
Music::position() const
{
    double pos = 0;
    if (_provider)
        _provider->GetPos(_provider, &pos);
    return pos;
}

float
Music::volume() const
{
    return _volume;
}

double
Music::length() const
{
    double len = 0;
    if (_provider)
        _provider->GetLength(_provider, &len);
    return len;
}

Music::TrackInfo
Music::trackInfo() const
{
    TrackInfo info;
    if (_provider)
    {
        FSTrackDescription tDesc;
        FSStreamDescription sDesc;

        _provider->GetTrackDescription(_provider, &tDesc);
        _provider->GetStreamDescription(_provider, &sDesc);

        info.artist = tDesc.artist;
        info.title = tDesc.title;
        info.album = tDesc.album;
        info.year = tDesc.year;
        info.genre = tDesc.genre;
        info.encoding = tDesc.encoding;
        info.bitrate = tDesc.bitrate / 1000;
        info.replayGain = tDesc.replaygain;
        info.sampleFormat = sDesc.samplerate;
        info.channels = sDesc.channels;
        info.sampleRate = FS_BITS_PER_SAMPLE(sDesc.sampleformat);
    }
    return info;
}

Music::MusicState
Music::status() const
{
    if (_provider)
    {
        FSMusicProviderStatus status = FMSTATE_UNKNOWN;
        _provider->GetStatus(_provider, &status);
        if (status == FMSTATE_PLAY)
            return Playing;
        else if (status == FMSTATE_STOP)
            return Stopped;
        else if (status == FMSTATE_FINISHED)
            return Finished;
        else
            return None;
    }
    return None;
}

void
Music::play()
{
    if (_provider)
        _provider->PlayToStream(_provider, _stream);
}

void
Music::stop()
{
    if (_provider)
        _provider->Stop(_provider);
}

void
Music::seekTo(double position)
{
    if (_provider)
        _provider->SeekTo(_provider, position);
}

void
Music::setVolume(float volume)
{
    if (_playback)
    {
        _volume = volume;
        _playback->SetVolume(_playback, _volume);
    }
}

void
Music::setRepeat(bool repeat)
{
    if (_provider)
    {
        _repeat = repeat;
        _provider->SetPlaybackFlags(_provider, FSMusicProviderPlaybackFlags(_repeat));
    }
}

void
Music::setFileName(const std::string& filename)
{
    if (FileSystem::fileExists(filename))
    {
        release();
        _fileName = filename;
        loadMusic();
    } else
        ILOG_ERROR(ILX_MUSIC, "%s not found!\n", filename.c_str());
}

bool
Music::loadMusic()
{
    ILOG_TRACE_F(ILX_MUSIC);
    DFBResult ret = SoundDFB::createMusicProvider(_fileName.c_str(), &_provider);
    if (!ret)
    {
        _provider->EnumTracks(_provider, track_display_callback, this);
        _provider->EnumTracks(_provider, track_playback_callback, this);
        return true;
    }
    return false;
}

DirectEnumerationResult
Music::playback(FSTrackID id, FSTrackDescription desc)
{
    ILOG_TRACE_F(ILX_MUSIC);
    FSStreamDescription s_dsc;

//    if (_provider->SelectTrack(_provider, id) != DR_OK)
//        return DENUM_OK;

    _provider->GetStreamDescription(_provider, &s_dsc);

    if (_stream)
    {
        FSStreamDescription dsc;
        /* Check whether _stream format changed. */
        _stream->GetDescription(_stream, &dsc);
        if (dsc.samplerate != s_dsc.samplerate || dsc.channels != s_dsc.channels || dsc.sampleformat != s_dsc.sampleformat)
        {
            if (_playback)
            {
                _playback->Release(_playback);
                _playback = NULL;
            }
            _stream->Release(_stream);
            _stream = NULL;
        }
    }

    if (!_stream)
    {
        /* Create the sound _stream and feed it. */
        if (SoundDFB::createStream(&s_dsc, &_stream) != DFB_OK)
            return DENUM_CANCEL;

        _stream->GetDescription(_stream, &s_dsc);
        _stream->GetPlayback(_stream, &_playback);
    }

    return DENUM_OK;
}

void
Music::release()
{
    ILOG_TRACE_F(ILX_MUSIC);
    if (_provider)
    {
        _provider->Release(_provider);
        _provider = NULL;
    }

    if (_stream)
    {
        _stream->Release(_stream);
        _stream = NULL;
    }

    if (_playback)
    {
        _playback->Release(_playback);
        _playback = NULL;
    }
}

} /* namespace ilixi */
