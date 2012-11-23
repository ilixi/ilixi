#ifndef ILIXI_VIDEOPLAYER_H_
#define ILIXI_VIDEOPLAYER_H_

#include <ui/Widget.h>
#include <types/Video.h>

namespace ilixi
{
class HBoxLayout;
class Label;
class Slider;
class ToolButton;

class VideoPlayer : public Widget
{
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

protected:
    virtual void
    compose(const PaintEvent& rect);

private:
    //! Video object.
    Video* _video;
    //! Subsurface for blitting video.
    IDirectFBSurface* _videoSurface;

    //! Playback time.
    Label* _time;
    //! Rewinds video.
    ToolButton* _rewind;
    //! Starts/stops playback.
    ToolButton* _play;
    //! Switches to fullscreen.
    ToolButton* _fullscreen;

    HBoxLayout* _line1;

    //! Controls video position.
    Slider* _position;
    //! Controls video volume.
    Slider* _volume;

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

    //! Converts seconds to HMS format.
    void
    toHMS(double secs, char* buffer);

    //! Blits frame to video surface.
    void
    updateVideo(IDirectFBSurface* frame);

    //! Updates widget geometry.
    void
    updateVPGeometry();
};

} /* namespace ilixi */
#endif /* ILIXI_VIDEOPLAYER_H_ */
