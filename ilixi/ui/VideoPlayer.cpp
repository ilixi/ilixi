#include <ui/VideoPlayer.h>
#include <ui/HBoxLayout.h>
#include <ui/Label.h>
#include <ui/ToolButton.h>
#include <ui/Slider.h>
#include <graphics/Painter.h>
#include <core/PlatformManager.h>
#include <core/Logger.h>
#include <lib/Util.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_VIDEOPLAYER, "ilixi/ui/VideoPlayer", "VideoPlayer");
D_DEBUG_DOMAIN( ILX_VIDEOPLAYERCTRL, "ilixi/ui/VideoPlayerControls", "VideoPlayerControls");

VideoPlayerControls::VideoPlayerControls(VideoPlayer* parent)
        : Widget(parent),
          _owner(parent)
{
    ILOG_TRACE_W(ILX_VIDEOPLAYERCTRL);
    setInputMethod(PointerPassthrough);
    _box = new HBoxLayout();
    _box->setVerticalAlignment(Alignment::Middle);
    _box->setSpacing(10);
    addChild(_box);

    _play = new ToolButton("Play");
    _play->setIcon(StyleHint::Play, Size(24, 24));
    _play->setToolButtonStyle(ToolButton::IconOnly);
    _play->setDisabled();
    _play->setDrawFrame(false);
    _play->sigClicked.connect(sigc::mem_fun(_owner, &VideoPlayer::playVideo));
    _box->addWidget(_play);

    _time = new Label("00:00");
    _box->addWidget(_time);

    _position = new Slider();
    _position->setDisabled();
    _position->sigValueChanged.connect(sigc::mem_fun(_owner, &VideoPlayer::seekVideo));
    _box->addWidget(_position);

    _dur = new Label("00:00");
    _box->addWidget(_dur);

    _volume = new Slider();
    _volume->setValue(100);
    _volume->setVisible(false);
    _volume->setXConstraint(MinimumConstraint);
    _volume->sigValueChanged.connect(sigc::mem_fun(_owner, &VideoPlayer::setVolume));
    _box->addWidget(_volume);

//    _fullscreen = new ToolButton("FullScreen");
//    _fullscreen->setIcon(StyleHint::ZoomIn, Size(24, 24));
//    _fullscreen->setToolButtonStyle(ToolButton::IconOnly);
//    _fullscreen->setDisabled();
//    _fullscreen->setDrawFrame(false);
//    //  _fullscreen->sigClicked.connect(sigc::mem_fun(this, &VideoPlayer::fullscreen));
//    _box->addWidget(_fullscreen);

    _anim.setDuration(500);
    _tween = new Tween(Tween::SINE, Tween::EASE_OUT, 0, 1);
    _anim.sigExec.connect(sigc::mem_fun(this, &VideoPlayerControls::tweenSlot));
    _anim.addTween(_tween);

    sigGeometryUpdated.connect(sigc::mem_fun(this, &VideoPlayerControls::updateVPCGeometry));
}

VideoPlayerControls::~VideoPlayerControls()
{
    ILOG_TRACE_W(ILX_VIDEOPLAYERCTRL);
}

Size
VideoPlayerControls::preferredSize() const
{
    return _box->preferredSize();
}

void
VideoPlayerControls::show()
{
    _anim.stop();
    _tween->setRange(_box->y(), 0);
    _anim.start();
}

void
VideoPlayerControls::hide()
{
    _anim.stop();
    _tween->setRange(_box->y(), height());
    _anim.start();
}

void
VideoPlayerControls::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    p.setBrush(Color(0, 0, 0, 128));
    p.fillRectangle(0, _box->y(), width(), height(), DSDRAW_BLEND);
}

void
VideoPlayerControls::tweenSlot()
{
    _box->setY(_tween->value());
    update();
}

void
VideoPlayerControls::updateVPCGeometry()
{
    _box->setGeometry(5, height(), width() - 10, height());
}

//*******************************************************************************************

VideoPlayer::VideoPlayer(Widget* parent)
        : Widget(parent),
          _video(NULL),
          _videoLSurface(NULL),
          _videoSurface(NULL),
          _videoFrame(NULL)
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    setInputMethod((WidgetInputMethod) (KeyPointer | PointerPassthrough));
    _controls = new VideoPlayerControls(this);
    addChild(_controls);

    sigGeometryUpdated.connect(sigc::mem_fun(this, &VideoPlayer::updateVPGeometry));
}

VideoPlayer::~VideoPlayer()
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    delete _video;
    if (_videoSurface)
        _videoSurface->Release(_videoSurface);
    if (_videoLSurface)
        _videoLSurface->Release(_videoLSurface);
}

int
VideoPlayer::heightForWidth(int width) const
{
    return -1;
}

Size
VideoPlayer::preferredSize() const
{
    return _controls->preferredSize();
}

void
VideoPlayer::load(const std::string& path)
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    delete _video;
    _videoFrame = NULL;
    _video = new Video(path);
    _video->sigFrameUpdated.connect(sigc::mem_fun(this, &VideoPlayer::updateVideo));
    _controls->_play->setEnabled();
    if (_video->hasAudio())
        _controls->_volume->setVisible(true);
    _controls->_dur->setText(toHMS(_video->length()));
}

void
VideoPlayer::keyDownEvent(const KeyEvent& keyEvent)
{
    switch (keyEvent.keySymbol)
    {
    case DIKS_CURSOR_LEFT:
    case DIKS_CURSOR_RIGHT:
    case DIKS_CURSOR_UP:
    case DIKS_CURSOR_DOWN:
        break;

    case DIKS_SPACE:
        _controls->_play->click(100);
        break;

    default:
        break;
    }
}

void
VideoPlayer::enterEvent(const PointerEvent& event)
{
    _controls->show();
}

void
VideoPlayer::leaveEvent(const PointerEvent& event)
{
    _controls->hide();
}

void
VideoPlayer::compose(const PaintEvent& event)
{
    if (_videoLSurface)
        surface()->clear();
    else if (_videoFrame)
    {
        DFBRectangle r = surfaceGeometry().dfbRect();
        surface()->dfbSurface()->GetSubSurface(surface()->dfbSurface(), &r, &_videoSurface);

        if (_videoSurface)
        {
            _videoSurface->StretchBlit(_videoSurface, _videoFrame, NULL, NULL);
            _videoSurface->Release(_videoSurface);
            _videoSurface = NULL;
        }
    } else
    {
        Painter p(this);
        p.begin(event);
        p.fillRectangle(0, 0, width(), height());
    }
}

void
VideoPlayer::playVideo()
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    switch (_video->status())
    {
    case DVSTATE_STOP:
        _controls->_play->setIcon(StyleHint::Pause, Size(24, 24));
        _controls->_play->update();
        _controls->_position->setEnabled();
        _video->play();
        break;

    case DVSTATE_FINISHED:
        _videoFrame = NULL;
        _controls->_play->setIcon(StyleHint::Play, Size(24, 24));
        _controls->_play->update();
        _controls->_position->setEnabled();
        _video->seek(0);
        break;

    case DVSTATE_BUFFERING:
    case DVSTATE_PLAY:
        _controls->_play->setIcon(StyleHint::Play, Size(24, 24));
        _controls->_play->update();
        _video->stop();
        break;

    default:
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
    _controls->_time->setText(toHMS(secs));
}

void
VideoPlayer::setVolume(int volume)
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    _video->setVolume(volume / 100.0);
}

void
VideoPlayer::updateVideo(IDirectFBSurface* frame)
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    _videoFrame = frame;

    if (!_controls->_position->pressed())
    {
        _controls->_position->setValue(100 * (_video->position() / _video->length()), false);
        _controls->_time->setText(toHMS(_video->position()));
    }

    if (_video->status() == DVSTATE_FINISHED)
    {
        _videoFrame = NULL;
        _controls->_play->setIcon(StyleHint::Play, Size(24, 24));
        _controls->_play->update();
    }

    if (_videoLSurface)
    {
        _videoLSurface->StretchBlit(_videoLSurface, _videoFrame, NULL, NULL);
        _videoLSurface->Flip(_videoLSurface, NULL, DSFLIP_ONSYNC);
    } else
        update();
}

void
VideoPlayer::updateVPGeometry()
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    Size s1 = _controls->preferredSize();
    _controls->setGeometry(0, height() - s1.height(), width(), s1.height());

    if (PlatformManager::instance().appOptions() & OptExclusive)
    {
        if (_videoLSurface)
        {
            DFBRectangle r = surfaceGeometry().dfbRect();
            _videoLSurface->MakeSubSurface(_videoLSurface, PlatformManager::instance().getLayerSurface("video"), &r);
        }

        if (surface() && !_videoLSurface)
        {
            IDirectFBSurface* video = PlatformManager::instance().getLayerSurface("video");
            if (video)
            {
                DFBRectangle r = surfaceGeometry().dfbRect();
                video->GetSubSurface(video, &r, &_videoLSurface);
            } else
            {
                ILOG_ERROR(ILX_VIDEOPLAYER, "Cannot get video layer surface!\n");
            }
        }
    } else if (PlatformManager::instance().appOptions() & OptDale)
    {
        // TODO video component
    } else
    {
        // if possible use video layer.
        // else use ui sub-surface
    }
}

} /* namespace ilixi */
