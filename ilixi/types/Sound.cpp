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

#include <types/Sound.h>
#include <lib/FileSystem.h>
#include <core/Logger.h>
#include <fcntl.h>
#include <unistd.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_SOUND, "ilixi/types/Sound", "Sound");

typedef struct fmtChunk
{
    u16 encoding;
    u16 channels;       //!< 1 = mono, 2 = stereo
    u32 frequency;      //!< One of 11025, 22050, or 44100 Hz
    u32 byterate;       //!< Average bytes per second
    u16 blockalign;     //!< Bytes per sample block
    u16 bitspersample;  //!< One of 8, 12, 16, or 4 for ADPCM
} fmtChunk;

static DirectResult
read_file_header(int fd)
{
    char buf[12];

    if (read(fd, buf, 12) < 12)
    {
        ILOG_ERROR(ILX_SOUND, "Could not read at least 12 bytes!\n");
        return DR_IO;
    }

    if (buf[0] != 'R' || buf[1] != 'I' || buf[2] != 'F' || buf[3] != 'F')
    {
        ILOG_ERROR(ILX_SOUND, "No RIFF header found!\n");
        return DR_UNSUPPORTED;
    }

    if (buf[8] != 'W' || buf[9] != 'A' || buf[10] != 'V' || buf[11] != 'E')
    {
        ILOG_ERROR(ILX_SOUND, "Not a WAVE!\n");
        return DR_UNSUPPORTED;
    }

    return DR_OK;
}

static int
read_chunk_header(int fd, char *magic)
{
    unsigned char buf[8];

    if (read(fd, buf, 8) < 8)
    {
        ILOG_ERROR(ILX_SOUND, "Could not read 8 bytes of chunk header!\n");
        return -1;
    }

    strncpy(magic, (char*) buf, 4);

    return buf[4] | (buf[5] << 8) | (buf[6] << 16) | (buf[7] << 24);

}

Sound::Sound()
        : _fileName(""),
          _buffer(NULL),
          _playback(NULL)
{
    ILOG_TRACE_F(ILX_SOUND);
}

Sound::Sound(const std::string& filename)
        : _fileName(filename),
          _buffer(NULL),
          _playback(NULL)
{
    ILOG_TRACE_F(ILX_SOUND);
    loadSample();
}

Sound::~Sound()
{
    ILOG_TRACE_F(ILX_SOUND);
    release();
}

void
Sound::start(bool looping)
{
    ILOG_TRACE_F(ILX_SOUND);
    if (_playback)
    {
        DirectResult ret = _playback->Start(_playback, 0, 0);
        if (ret)
            ILOG_ERROR(ILX_SOUND, "Could not start playback!\n");
    } else
        ILOG_ERROR(ILX_SOUND, "Playback interface not ready!\n");
}

void
Sound::stop()
{
    ILOG_TRACE_F(ILX_SOUND);
    if (_playback)
    {
        DirectResult ret = _playback->Stop(_playback);
        if (ret)
            ILOG_ERROR(ILX_SOUND, "Could not stop playback!\n");
    } else
        ILOG_ERROR(ILX_SOUND, "Playback interface not ready!\n");
}

void
Sound::setVolume(float level)
{
    ILOG_TRACE_F(ILX_SOUND);
    if (_playback)
    {
        if (level < 0)
            level = 0;
        else if (level > 1)
            level = 1;
        DirectResult ret = _playback->SetVolume(_playback, level);
        if (ret)
            ILOG_ERROR(ILX_SOUND, "Could not set playback volume level %f!\n", level);
    } else
        ILOG_ERROR(ILX_SOUND, "Playback interface not ready!\n");
}

void
Sound::setPan(float pan)
{
    ILOG_TRACE_F(ILX_SOUND);
    if (_playback)
    {
        if (pan < -1)
            pan = -1;
        else if (pan > 1)
            pan = 1;
        DirectResult ret = _playback->SetPan(_playback, pan);
        if (ret)
            ILOG_ERROR(ILX_SOUND, "Could not set pan value %f!\n", pan);
    } else
        ILOG_ERROR(ILX_SOUND, "Playback interface not ready!\n");
}

void
Sound::setPitch(float value)
{
    ILOG_TRACE_F(ILX_SOUND);
    if (_playback)
    {
        if (value < -1)
            value = -1;
        else if (value > 1)
            value = 1;
        DirectResult ret = _playback->SetPitch(_playback, value);
        if (ret)
            ILOG_ERROR(ILX_SOUND, "Could not set pitch value %f!\n", value);
    } else
        ILOG_ERROR(ILX_SOUND, "Playback interface not ready!\n");
}

void
Sound::setDirection(PlaybackDirection direction)
{
    ILOG_TRACE_F(ILX_SOUND);
    if (_playback)
    {
        DirectResult ret = _playback->SetDirection(_playback, (FSPlaybackDirection) direction);
        if (ret)
            ILOG_ERROR(ILX_SOUND, "Could not set playback direction!\n");
    } else
        ILOG_ERROR(ILX_SOUND, "Playback interface not ready!\n");
}

void
Sound::setFileName(const std::string& filename)
{
    if (FileSystem::fileExists(filename))
    {
        release();
        _fileName = filename;
        loadSample();
    } else
        ILOG_ERROR(ILX_SOUND, "%s not found!\n", filename.c_str());
}

void
Sound::loadSample()
{
    ILOG_TRACE_F(ILX_SOUND);
    DirectResult ret;
    int fd;
    FSBufferDescription desc;
    void *data;
    fmtChunk fmt;
    int len;

    fd = open(_fileName.c_str(), O_RDONLY);
    if (fd < 0)
    {
        ILOG_ERROR(ILX_SOUND, "Could not open %s!\n", _fileName.c_str());
        return;
    }

    if (read_file_header(fd))
    {
        close(fd);
        return;
    }

    while (1)
    {
        char magic[4];

        len = read_chunk_header(fd, magic);
        if (len <= 0)
        {
            ILOG_ERROR(ILX_SOUND, "Could not find format chunk!\n");
            close(fd);
            return;
        }

        if (magic[0] == 'f' || magic[1] == 'm' || magic[2] == 't')
        {
            if (len < sizeof(fmtChunk))
            {
                ILOG_ERROR(ILX_SOUND, "Format chunk has invalid size (%d/%zu)!\n", len, sizeof(fmtChunk));
                close(fd);
                return;
            }

            if (read(fd, &fmt, sizeof(fmtChunk)) < sizeof(fmtChunk))
            {
                ILOG_ERROR(ILX_SOUND, "Could not read format chunk!\n");
                close(fd);
                return;
            }

            if (lseek(fd, len - sizeof(fmtChunk), SEEK_CUR) == (off_t) -1)
            {
                ILOG_ERROR(ILX_SOUND, "Could not seek past chunk!\n");
                close(fd);
                return;
            }

            break;
        } else
        {
            if (lseek(fd, len, SEEK_CUR) == (off_t) -1)
            {
                ILOG_ERROR(ILX_SOUND, "Could not seek past chunk!\n");
                close(fd);
                return;
            }
        }
    }

#ifdef WORDS_BIGENDIAN
    fixup_fmtchunk( &fmt );
#endif

    if (fmt.encoding != 1)
    {
        ILOG_ERROR(ILX_SOUND, "Only PCM supported, yet!\n");
        close(fd);
        return;
    }

    if (fmt.bitspersample != 16 && fmt.bitspersample != 8)
    {
        ILOG_ERROR(ILX_SOUND, "Only 16 or 8 bit supported, yet!\n");
        close(fd);
        return;
    }

    desc.flags = (FSBufferDescriptionFlags) (FSBDF_LENGTH | FSBDF_CHANNELS | FSBDF_SAMPLEFORMAT | FSBDF_SAMPLERATE);
    desc.channels = fmt.channels;
    desc.sampleformat = (fmt.bitspersample == 8) ? FSSF_U8 : FSSF_S16;
    desc.samplerate = fmt.frequency;

    while (1)
    {
        char magic[4];

        len = read_chunk_header(fd, magic);
        if (len <= 0)
        {
            ILOG_ERROR(ILX_SOUND, "Could not find data chunk!\n");
            close(fd);
            return;
        }

        if (magic[0] == 'd' && magic[1] == 'a' && magic[2] == 't' && magic[3] == 'a')
        {
            desc.length = len / fmt.blockalign;
            break;
        } else
        {
            if (lseek(fd, len, SEEK_CUR) == (off_t) -1)
            {
                ILOG_ERROR(ILX_SOUND, "Could not seek past chunk!\n");
                close(fd);
                return;
            }
        }
    }

    DFBResult res = SoundDFB::createBuffer(&desc, &_buffer);
    if (res)
    {
        ILOG_ERROR(ILX_SOUND, "Error in IFusionSound::CreateBuffer\n");
        close(fd);
        return;
    }

    _buffer->Lock(_buffer, &data, 0, 0);

    if (read(fd, data, len) < len)
        ILOG_ERROR(ILX_SOUND, "Warning: Could not read all data bytes!\n");

#ifdef WORDS_BIGENDIAN
    if (fmt.bitspersample == 16)
    fixup_sampledata (data, len);
#endif

    close(fd);
    _buffer->Unlock(_buffer);

    ret = _buffer->CreatePlayback(_buffer, &_playback);
    if (ret)
        ILOG_ERROR(ILX_SOUND, "Error in IFusionSoundBuffer::CreatePlayback\n");
}

void
Sound::release()
{
    if (_playback)
        _playback->Release(_playback);

    if (_buffer)
        _buffer->Release(_buffer);
}

} /* namespace ilixi */
