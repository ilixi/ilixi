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

#ifndef ILIXI_VIDEO_H_
#define ILIXI_VIDEO_H_

#include <directfb.h>
#include <sigc++/signal.h>
#include <string>

namespace ilixi
{
//! Loads and plays video using DirectFB video providers.
class Video : virtual public sigc::trackable
{
    friend void
    renderToSurface(void *);

public:

    /*!
     * Constructor.
     */
    Video(const std::string& path);

    /*!
     * Destructor.
     */
    virtual
    ~Video();

    /*!
     * Returns true if video has audio.
     */
    bool
    hasAudio() const;

    /*!
     * Returns playback position in seconds.
     */
    double
    position() const;

    /*!
     * Returns length of video in seconds.
     */
    double
    length() const;

    /*!
     * Returns playback speed.
     */
    double
    speed() const;

    /*!
     * Returns volume of audio.
     */
    float
    volume() const;

    /*!
     * Returns playback status.
     */
    DFBVideoProviderStatus
    status() const;

    /*!
     * Returns encoding of video.
     */
    const char*
    encodingVideo() const;

    /*!
     * Returns original width of video.
     */
    int
    width() const;

    /*!
     * Returns original height of video.
     */
    int
    height() const;

    /*!
     * Returns aspect ratio of video.
     */
    double
    aspect() const;

    /*!
     * Returns fps of video in fps.
     */
    double
    framerate() const;

    /*!
     * Returns bitrate of video in Kbits/s.
     */
    int
    bitrateVideo() const;

    /*!
     * Returns encoding of audio.
     */
    const char*
    encodingAudio() const;

    /*!
     * Returns sample rate of audio in KHz.
     */
    int
    samplerate() const;

    /*!
     * Returns number of channels in audio.
     */
    int
    channels() const;

    /*!
     * Returns bitrate of audio in Kbits/s.
     */
    int
    bitrateAudio() const;

    /*!
     * Returns title field of video.
     */
    const char*
    title() const;

    /*!
     * Returns author field of video.
     */
    const char*
    author() const;

    /*!
     * Returns album field of video.
     */
    const char*
    album() const;

    /*!
     * Returns year field of video.
     */
    int
    year() const;

    /*!
     * Returns genre field of video.
     */
    const char*
    genre() const;

    /*!
     * Returns comment field of video.
     */
    const char*
    comment() const;

    /*!
     * Returns video frame.
     */
    IDirectFBSurface*
    frame() const;

    /*!
     * Loads a video file. Returns true if successful.
     */
    bool
    load(const std::string& path);

    /*!
     * Starts playing video.
     */
    void
    play();

    /*!
     * Pauses video.
     */
    void
    pause();

    /*!
     * Seeks to given position.
     */
    void
    seek(double secs);

    /*!
     * Stops playing video.
     */
    void
    stop();

    /*!
     * Sets playback speed.
     * @param speed
     */
    void
    setSpeed(double speed);

    /*!
     * Sets audio volume.
     */
    void
    setVolume(float level);

    /*!
     * Enables or disables signaling of events.
     *
     * @param eventsEnabled if true attaches a buffer and listens for events.
     */
    void
    setEventsEnabled(bool eventsEnabled);

    /*!
     * Enable/Disable automatic restart of playback when stream ends.
     *
     * @param looping if true, play video in gapless looping mode.
     */
    void
    setLooping(bool looping);

    /*!
     * Returns video and audio information.
     */
    std::string
    toString();

    //! This signal is emitted when a new frame is available.
    sigc::signal<void, IDirectFBSurface*> sigFrameUpdated;

private:
    //! DFB video provider.
    IDirectFBVideoProvider* _provider;
    //! Video surface.
    IDirectFBSurface* _frame;
    //! Event buffer.
    IDirectFBEventBuffer* _buffer;

    //! Path to video file.
    std::string _path;
    //! DFB stream description for video.
    DFBStreamDescription _streamDesc;
    //! DFB surface description for video.
    DFBSurfaceDescription _surfaceDesc;
    //! DFB video provider capabilities.
    DFBVideoProviderCapabilities _providerCaps;
};

} /* namespace ilixi */

#endif /* ILIXI_VIDEO_H_ */
