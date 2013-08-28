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
    _play->setIcon(StyleHint::Play, Size(16, 16));
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
    if (_box->y() == height())
    {
        _anim.stop();
        _tween->setRange(_box->y(), 0);
        _anim.start();
    }
}

void
VideoPlayerControls::hide()
{
    if (_box->y() == 0)
    {
        _anim.stop();
        _tween->setRange(_box->y(), height());
        _anim.start();
    }
}

void
VideoPlayerControls::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    stylist()->drawHeader(&p, 0, _box->y(), width(), height());
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
          _videoFrame(NULL),
          _flags(AutoHideControls)
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    setInputMethod((WidgetInputMethod) (KeyPointerTracking | PointerPassthrough));
    _controls = new VideoPlayerControls(this);
    addChild(_controls);

    _timer.setInterval(1000);
    _timer.sigExec.connect(sigc::mem_fun(_controls, &VideoPlayerControls::hide));
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

bool
VideoPlayer::load(const std::string& path)
{
    ILOG_TRACE_W(ILX_VIDEOPLAYER);
    ILOG_DEBUG(ILX_VIDEOPLAYER, " -> path: %s\n", path.c_str());
    delete _video;
    _videoFrame = NULL;
    _video = new Video(path);
    if (_video->status() != DVSTATE_UNKNOWN)
    {
        _video->sigFrameUpdated.connect(sigc::mem_fun(this, &VideoPlayer::updateVideo));
        _controls->_play->setEnabled();
        if (_video->hasAudio())
            _controls->_volume->setVisible(true);
        _controls->_dur->setText(toHMS(_video->length()));
        return true;
    } else
    {
        delete _video;
        _video = NULL;
    }
    return false;
}

std::string
VideoPlayer::info() const
{
    if (_video)
        return _video->toString();
    return "N/A";
}

void
VideoPlayer::setAutoHideControls(bool autoHide)
{
    if (autoHide)
    {
        _flags = (VideoPlayerFlags) (_flags | AutoHideControls);
        _controls->hide();
    } else
    {
        _flags = (VideoPlayerFlags) (_flags & ~AutoHideControls);
        _controls->show();
    }
}

void
VideoPlayer::setKeepAspectRatio(bool keepAspect)
{
    if (keepAspect)
        _flags = (VideoPlayerFlags) (_flags | KeepAspectRatio);
    else
        _flags = (VideoPlayerFlags) (_flags & ~KeepAspectRatio);
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
VideoPlayer::pointerMotionEvent(const PointerEvent& event)
{
    if (_flags & AutoHideControls)
    {
        _controls->show();
        if (_timer.running())
            _timer.stop();
        _timer.start(2000, 1);
    }
}

void
VideoPlayer::compose(const PaintEvent& event)
{
    if (_videoLSurface)
        surface()->clear();
    else if (_videoFrame)
    {
        DFBRectangle r = frameGeometry().dfbRect();
        if (!(_flags & AutoHideControls))
            r.h -= _controls->height();

//        if (_flags & KeepAspectRatio)
//        {
//        }

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
    if (!_video)
        return;

    switch (_video->status())
    {
    case DVSTATE_STOP:
        _controls->_play->setIcon(StyleHint::Pause, Size(16, 16));
        _controls->_play->update();
        _controls->_position->setEnabled();
        _video->play();
        break;

    case DVSTATE_FINISHED:
        _videoFrame = NULL;
        _controls->_play->setIcon(StyleHint::Play, Size(16, 16));
        _controls->_play->update();
        _controls->_position->setEnabled();
        _video->seek(0);
        break;

    case DVSTATE_BUFFERING:
    case DVSTATE_PLAY:
        _controls->_play->setIcon(StyleHint::Play, Size(16, 16));
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

    if (_video->status() != DVSTATE_PLAY)
    {
        _videoFrame = NULL;
        _controls->_play->setIcon(StyleHint::Play, Size(16, 16));
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
    _controls->setGeometry(0, height() - s1.height() - 10, width(), s1.height() + 10);

    if (PlatformManager::instance().appOptions() & OptExclusive)
    {
        if (_videoLSurface)
        {
            DFBRectangle r = frameGeometry().dfbRect();
            if (!(_flags & AutoHideControls))
                r.h -= _controls->height();

            _videoLSurface->MakeSubSurface(_videoLSurface, PlatformManager::instance().getLayerSurface("video"), &r);
        }

        if (surface() && !_videoLSurface)
        {
            IDirectFBSurface* video = PlatformManager::instance().getLayerSurface("video");
            if (video)
            {
                DFBRectangle r = frameGeometry().dfbRect();
                if (!(_flags & AutoHideControls))
                    r.h -= _controls->height();

                video->GetSubSurface(video, &r, &_videoLSurface);
            } else
            {
                ILOG_ERROR(ILX_VIDEOPLAYER, "Cannot get video layer surface!\n");
            }
        }
    } else if (PlatformManager::instance().appOptions() & OptDaleAuto)
    {
        // TODO video component
    } else
    {
        // if possible use video layer.
        // else use ui sub-surface
    }
}

} /* namespace ilixi */
