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

#ifndef ILIXI_VIDEOPLAYER_H_
#define ILIXI_VIDEOPLAYER_H_

#include <ui/Widget.h>
#include <types/Video.h>
#include <lib/TweenAnimation.h>
#include <lib/Timer.h>

namespace ilixi
{
class HBoxLayout;
class Label;
class Slider;
class ToolButton;
class VideoPlayer;

//! Provides controls for a VideoPlayer widget.
class VideoPlayerControls : public Widget
{
    friend class VideoPlayer;
public:
    /*!
     * Contructor.
     */
    VideoPlayerControls(VideoPlayer* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~VideoPlayerControls();

    Size
    preferredSize() const;

    void
    show();

    void
    hide();

protected:
    void
    compose(const PaintEvent& event);

private:
    //! Owner.
    VideoPlayer* _owner;
    //! Layout for child widgets.
    HBoxLayout* _box;
    //! Playback position.
    Label* _time;
    //! Video duration.
    Label* _dur;
    //! Starts/stops playback.
    ToolButton* _play;
    //! Controls video position.
    Slider* _position;
    //! Controls video volume.
    Slider* _volume;
    //! Animation for showing/hiding control box.
    TweenAnimation _anim;
    Tween* _tween;

    void
    tweenSlot();

    void
    updateVPCGeometry();
};

//! Provides a simple video player widget.
class VideoPlayer : public Widget
{
    friend class VideoPlayerControls;
public:
    /*!
     * Constructor.
     */
    VideoPlayer(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~VideoPlayer();

    int
    heightForWidth(int width) const;

    Size
    preferredSize() const;

    /*!
     * Loads video for playback.
     *
     * @param path to video file.
     */
    bool
    load(const std::string& path);

    /*!
     * Returns a string with information about video file.
     */
    std::string
    info() const;

    /*!
     * Sets whether controls are always shown.
     *
     * @param autoHide if true controls will be hidden automatically.
     */
    void
    setAutoHideControls(bool autoHide);

    void
    setKeepAspectRatio(bool keepAspect);

protected:
    virtual void
    keyDownEvent(const KeyEvent& keyEvent);

    virtual void
    pointerMotionEvent(const PointerEvent& event);

    virtual void
    compose(const PaintEvent& event);

private:
    enum VideoPlayerFlags
    {
        None = 0x001,
        AutoHideControls = 0x002,
        Looping = 0x004,
        KeepAspectRatio = 0x008
    };

    //! Video object.
    Video* _video;
    //! Layer sub-surface used for blitting video frames, only available in exclusive mode.
    IDirectFBSurface* _videoLSurface;
    //! Direct sub-surface used for blitting video frames.
    IDirectFBSurface* _videoSurface;
    //! Current video frame.
    IDirectFBSurface* _videoFrame;
    //! Video player controls.
    VideoPlayerControls* _controls;
    //! Timer used for toggling controls.
    Timer _timer;
    //! This property stores various attributes for video player.
    VideoPlayerFlags _flags;

    //! Starts video playback.
    void
    playVideo();

    //! Seeks video to start.
    void
    rewindVideo();

    //! Seeks video to given position.
    void
    seekVideo(int position);

    //! Sets video volume.
    void
    setVolume(int volume);

    //! Blits frame to video surface.
    void
    updateVideo(IDirectFBSurface* frame);

    //! Updates widget geometry.
    void
    updateVPGeometry();
};

} /* namespace ilixi */
#endif /* ILIXI_VIDEOPLAYER_H_ */
