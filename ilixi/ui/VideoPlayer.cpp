#include <ui/VideoPlayer.h>
#include <ui/HBoxLayout.h>
#include <ui/Label.h>
#include <ui/ToolButton.h>
#include <ui/Slider.h>
#include <graphics/Painter.h>
#include <core/PlatformManager.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_VIDEOPLAYER, "ilixi/ui/VideoPlayer", "VideoPlayer");

VideoPlayer::VideoPlayer(Widget* parent)
        : Widget(parent),
          _video(NULL),
          _videoSurface(NULL)
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    _line1 = new HBoxLayout();
    addChild(_line1);

    _rewind = new ToolButton("Rewind");
    _rewind->setIcon(StyleHint::Prev);
    _rewind->setToolButtonStyle(ToolButton::IconOnly);
    _rewind->setDisabled();
    _rewind->sigClicked.connect(sigc::mem_fun(this, &VideoPlayer::rewindVideo));
    _line1->addWidget(_rewind);

    _play = new ToolButton("Play");
    _play->setIcon(StyleHint::Play);
    _play->setToolButtonStyle(ToolButton::IconOnly);
    _play->setDisabled();
    _play->sigClicked.connect(sigc::mem_fun(this, &VideoPlayer::playVideo));
    _line1->addWidget(_play);

    _position = new Slider();
    _position->setDisabled();
    _line1->addWidget(_position);

    _position->sigValueChanged.connect(sigc::mem_fun(this, &VideoPlayer::seekVideo));

    _time = new Label("00:00:00");
    _line1->addWidget(_time);

    _fullscreen = new ToolButton("FullScreen");
    _fullscreen->setIcon(StyleHint::ZoomIn);
    _fullscreen->setToolButtonStyle(ToolButton::IconOnly);
    _fullscreen->setDisabled();
    //  _fullscreen->sigClicked.connect(sigc::mem_fun(this, &VideoPlayer::fullscreen));
    _line1->addWidget(_fullscreen);

    _volume = new Slider();
    _volume->setValue(100);
    _volume->setDisabled();
    _volume->setXConstraint(MinimumConstraint);
    _volume->sigValueChanged.connect(sigc::mem_fun(this, &VideoPlayer::setVolume));
    _line1->addWidget(_volume);

    sigGeometryUpdated.connect(sigc::mem_fun(this, &VideoPlayer::updateVPGeometry));
    setInputMethod(KeyAndPointerInputTracking);
}

VideoPlayer::~VideoPlayer()
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    delete _video;
    if (_videoSurface)
        _videoSurface->Release(_videoSurface);
}

int
VideoPlayer::heightForWidth(int width) const
{
    return -1;
}

Size
VideoPlayer::preferredSize() const
{
    Size s1 = _line1->preferredSize();
    return s1;
}

void
VideoPlayer::load(const std::string& path)
{
    delete _video;
    _video = new Video(path);
    _video->frameUpdated.connect(sigc::mem_fun(this, &VideoPlayer::updateVideo));
    _play->setEnabled();
}

void
VideoPlayer::compose(const PaintEvent& rect)
{
    Painter painter(this);
    painter.begin(rect);
    painter.drawRectangle(0, 0, width(), height());
    painter.end();
}

void
VideoPlayer::playVideo()
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    switch (_video->status())
    {
    case DVSTATE_STOP:
        _video->play();
        _play->setIcon(StyleHint::Pause);
        //    _play->update();
        _rewind->setEnabled();
        _fullscreen->setEnabled();
        _position->setEnabled();
        _volume->setEnabled();
        break;
    case DVSTATE_FINISHED:
        _video->seek(0);
        _video->play();
        _play->setIcon(StyleHint::Pause);
        //    _play->update();
        _rewind->setEnabled();
        _fullscreen->setEnabled();
        _position->setEnabled();
        _volume->setEnabled();
        break;

    case DVSTATE_BUFFERING:
    case DVSTATE_PLAY:
        _video->stop();
        _play->setIcon(StyleHint::Play);
        //    _play->update();
        break;

    default:
        _play->setDisabled();
        break;
    }
}

void
VideoPlayer::rewindVideo()
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    _video->seek(0);
}

void
VideoPlayer::seekVideo(int position)
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    double secs = (position * _video->length()) / 100;
    _video->seek(secs);
    char buffer[9];
    toHMS(secs, buffer);
    _time->setText(buffer);
}

void
VideoPlayer::setVolume(int volume)
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    _video->setVolume(volume / 100.0);
}

void
VideoPlayer::toHMS(double secs, char* buffer)
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    int hour, min, sec;
    hour = secs / 3600;
    secs = (int) secs % 3600;
    min = secs / 60;
    secs = (int) secs % 60;
    sec = secs;
    sprintf(buffer, "%02d:%02d:%02d", hour, min, sec);
}

void
VideoPlayer::updateVideo(IDirectFBSurface* frame)
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    _videoSurface->StretchBlit(_videoSurface, frame, NULL, NULL);
    _videoSurface->Flip(_videoSurface, NULL, DSFLIP_ONSYNC);

    if (!_position->pressed())
    {
        char buffer[9];
        toHMS(_video->position(), buffer);
        _time->setText(buffer);
        _position->setValue(100 * (_video->position() / _video->length()), false);
    }

    if (_video->status() == DVSTATE_FINISHED)
    {
        _play->setIcon(StyleHint::Play);
        _play->update();
    }
}

void
VideoPlayer::updateVPGeometry()
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    Size s1 = _line1->preferredSize();
    _line1->setGeometry(5, height() - s1.height() - 5, width() - 10, s1.height());

    if (_videoSurface)
    {
        DFBRectangle r = surfaceGeometry().dfbRect();
        _videoSurface->MakeSubSurface(_videoSurface, PlatformManager::instance().getLayerSurface("video"), &r);
    }

    if (surface() && !_videoSurface)
    {
        IDirectFBSurface* video = PlatformManager::instance().getLayerSurface("video");
        if (video)
        {
            DFBRectangle r = surfaceGeometry().dfbRect();
            video->GetSubSurface(video, &r, &_videoSurface);
        } else
        {
            ILOG_ERROR(ILX_VIDEOPLAYER, "Cannot get video layer surface!\n");
        }
    }
}

} /* namespace ilixi */
