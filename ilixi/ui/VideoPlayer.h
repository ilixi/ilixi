#ifndef ILIXI_VIDEOPLAYER_H_
#define ILIXI_VIDEOPLAYER_H_

#include <ui/Widget.h>
#include <types/Video.h>
#include <lib/TweenAnimation.h>

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
    //! Switches to fullscreen.
    ToolButton* _fullscreen;
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
    void
    load(const std::string& path);

    sigc::signal<void> sigLoaded;

protected:
    virtual void
    keyDownEvent(const KeyEvent& keyEvent);

    virtual void
    enterEvent(const PointerEvent& event);

    virtual void
    leaveEvent(const PointerEvent& event);

    virtual void
    compose(const PaintEvent& event);

private:
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
