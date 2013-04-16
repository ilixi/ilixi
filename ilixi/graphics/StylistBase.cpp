/*
 Copyright 2010-2012 Tarik Sekmen.

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

#include <graphics/StylistBase.h>
#include <lib/TweenAnimation.h>
#include <sigc++/bind.h>
#include <ui/Widget.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_STYLISTBASE, "ilixi/graphics/StylistBase", "StylistBase");

Image* StylistBase::_noImage = NULL;

StylistBase::StylistBase()
        : _fonts(NULL),
          _icons(NULL),
          _palette(NULL),
          _style(NULL)
{
    ILOG_TRACE(ILX_STYLISTBASE);
    _noImage = new Image(ILIXI_DATADIR"images/noImage.png", 48, 48);
    //initAnimations();
}

StylistBase::~StylistBase()
{
    delete _fonts;
    delete _icons;
    delete _palette;
    delete _style;
    delete _noImage;
}

bool
StylistBase::setFontPack(const char* fontPack)
{
    if (_fonts)
        return _fonts->parseFonts(fontPack);
    return false;
}

bool
StylistBase::setIconPack(const char* iconPack)
{
    if (_icons)
        return _icons->parseIcons(iconPack);
    return false;
}

bool
StylistBase::setPaletteFromFile(const char* palette)
{
    if (_palette)
        return _palette->parsePalette(palette);
    return false;
}

bool
StylistBase::setStyleFromFile(const char* style)
{
    if (_style)
        return _style->parseStyle(style);
    return false;
}

Size
StylistBase::defaultSize(StyleHint::Size size) const
{
    switch (size)
    {
    case StyleHint::PushButton:
        return Size(defaultParameter(StyleHint::PushButtonLR), _style->pb.def.m.height());
    case StyleHint::ProgressBar:
        return Size(100, _style->pr.def.m.height());
    case StyleHint::Slider:
        return Size(100, _style->hSl.def.m.height());
    case StyleHint::SliderV:
        return Size(_style->vSl.def.m.width(), 100);
    default:
        return Size();
    }
}

int
StylistBase::defaultParameter(StyleHint::Parameter parameter) const
{
    switch (parameter)
    {
    case StyleHint::PushButtonLeft:
        return _style->pb.def.l.width();

    case StyleHint::PushButtonRight:
        return _style->pb.def.r.width();

    case StyleHint::PushButtonLR:
        return _style->pb.def.l.width() + _style->pb.def.r.width();

    case StyleHint::PushButtonHeight:
        return _style->pb.def.l.height();

    case StyleHint::CheckBoxHeight:
        return _style->cb.def.height();
    case StyleHint::CheckBoxWidth:
        return _style->cb.def.width();

    case StyleHint::RadioHeight:
        return _style->rbOn.def.height();
    case StyleHint::RadioWidth:
        return _style->rbOn.def.width();

    case StyleHint::ButtonOffset:
        return 5;

    case StyleHint::FrameOffsetLeft:
        return _style->fr.def.l.width();
    case StyleHint::FrameOffsetRight:
        return _style->fr.def.r.width();
    case StyleHint::FrameOffsetTop:
        return _style->fr.def.tm.height();
    case StyleHint::FrameOffsetBottom:
        return _style->fr.def.bm.height();
    case StyleHint::FrameOffsetLR:
        return _style->fr.def.l.width() + _style->fr.def.r.width();
    case StyleHint::FrameOffsetTB:
        return _style->fr.def.tm.height() + _style->fr.def.bm.height();

    case StyleHint::TabOffsetLeft:
        return _style->box.def.l.width();
    case StyleHint::TabOffsetRight:
        return _style->box.def.r.width();
    case StyleHint::TabOffsetTop:
        return _style->box.def.tm.height();
    case StyleHint::TabOffsetBottom:
        return _style->box.def.bm.height();
    case StyleHint::TabOffsetLR:
        return _style->box.def.l.width() + _style->box.def.r.width();
    case StyleHint::TabOffsetTB:
        return _style->box.def.tm.height() + _style->box.def.bm.height();

    case StyleHint::LineInputLeft:
        return _style->li.def.l.width();
    case StyleHint::LineInputRight:
        return _style->li.def.r.width();
    case StyleHint::LineInputLR:
        return _style->li.def.l.width() + _style->li.def.l.width();
    case StyleHint::LineInputTop:
        return _style->li.def.tl.height();
    case StyleHint::LineInputBottom:
        return _style->li.def.bl.height();
    case StyleHint::LineInputTB:
        return _style->li.def.tl.height() + _style->li.def.bl.height();

    case StyleHint::ToolButtonLeft:
        return _style->tb.def.l.width();
    case StyleHint::ToolButtonRight:
        return _style->tb.def.r.width();
    case StyleHint::ToolButtonLR:
        return _style->tb.def.l.width() + _style->tb.def.r.width();
    case StyleHint::ToolButtonTop:
        return _style->tb.def.tm.height();
    case StyleHint::ToolButtonBottom:
        return _style->tb.def.bm.height();
    case StyleHint::ToolButtonTB:
        return _style->tb.def.tm.height() + _style->tb.def.bm.height();

    case StyleHint::ToolButtonIndicator:
        return 8;

    case StyleHint::SliderIndicatorWidth:
        return _style->slI.def.width();
    case StyleHint::SliderIndicatorHeight:
        return _style->slI.def.height();

    case StyleHint::ScrollBarHeight:
        return _style->hScr.m.height();
    case StyleHint::ScrollBarWidth:
        return _style->vScr.m.width();

    default:
        return -1;
    }
}

Font*
StylistBase::defaultFont(StyleHint::Font font) const
{
    return _fonts->getFont(font);
}

Image*
StylistBase::defaultIcon(StyleHint::PackedIcon icon) const
{
    ILOG_TRACE(ILX_STYLISTBASE);
    switch (icon)
    {
    case StyleHint::ArrowNE:
        return _icons->getIcon("arrow_ne");
    case StyleHint::ArrowSW:
        return _icons->getIcon("arrow_sw");
    case StyleHint::Calendar:
        return _icons->getIcon("calendar");
    case StyleHint::Clock:
        return _icons->getIcon("clock");
    case StyleHint::Comment:
        return _icons->getIcon("comment");
    case StyleHint::CPU:
        return _icons->getIcon("cpu");
    case StyleHint::Cross:
        return _icons->getIcon("cross");
    case StyleHint::CrossCircle:
        return _icons->getIcon("cross_circle");
    case StyleHint::Down:
        return _icons->getIcon("arrow_down");
    case StyleHint::Eject:
        return _icons->getIcon("eject");
    case StyleHint::File:
        return _icons->getIcon("file");
    case StyleHint::FileDelete:
        return _icons->getIcon("file_delete");
    case StyleHint::FileDownload:
        return _icons->getIcon("file_download");
    case StyleHint::FileNew:
        return _icons->getIcon("file_new");
    case StyleHint::FileText:
        return _icons->getIcon("file_text");
    case StyleHint::FileUpload:
        return _icons->getIcon("file_upload");
    case StyleHint::Flag:
        return _icons->getIcon("flag");
    case StyleHint::Heart:
        return _icons->getIcon("heart");
    case StyleHint::Info:
        return _icons->getIcon("info");
    case StyleHint::Left:
        return _icons->getIcon("arrow_left");
    case StyleHint::Minus:
        return _icons->getIcon("minus");
    case StyleHint::Movie:
        return _icons->getIcon("movie");
    case StyleHint::Music:
        return _icons->getIcon("music");
    case StyleHint::Network:
        return _icons->getIcon("network");
    case StyleHint::Next:
        return _icons->getIcon("next");
    case StyleHint::Next2:
        return _icons->getIcon("next2");
    case StyleHint::Pause:
        return _icons->getIcon("pause");
    case StyleHint::Phone:
        return _icons->getIcon("phone");
    case StyleHint::Picture:
        return _icons->getIcon("picture");
    case StyleHint::Play:
        return _icons->getIcon("play");
    case StyleHint::Plus:
        return _icons->getIcon("plus");
    case StyleHint::Power:
        return _icons->getIcon("power");
    case StyleHint::Prev:
        return _icons->getIcon("prev");
    case StyleHint::Prev2:
        return _icons->getIcon("prev2");
    case StyleHint::RAM:
        return _icons->getIcon("ram");
    case StyleHint::Restart:
        return _icons->getIcon("restart");
    case StyleHint::Ribbon:
        return _icons->getIcon("ribbon");
    case StyleHint::Right:
        return _icons->getIcon("arrow_right");
    case StyleHint::Search:
        return _icons->getIcon("search");
    case StyleHint::Settings:
        return _icons->getIcon("settings");
    case StyleHint::Share:
        return _icons->getIcon("share");
    case StyleHint::Standby:
        return _icons->getIcon("standby");
    case StyleHint::Star:
        return _icons->getIcon("star");
    case StyleHint::Stats:
        return _icons->getIcon("stats");
    case StyleHint::Stop:
        return _icons->getIcon("stop");
    case StyleHint::Tag:
        return _icons->getIcon("tag");
    case StyleHint::ThumbDown:
        return _icons->getIcon("thumb_down");
    case StyleHint::ThumbUp:
        return _icons->getIcon("thumb_up");
    case StyleHint::Tick:
        return _icons->getIcon("tick");
    case StyleHint::TickCircle:
        return _icons->getIcon("tick_circle");
    case StyleHint::Up:
        return _icons->getIcon("arrow_up");
    case StyleHint::ZoomIn:
        return _icons->getIcon("zoom_in");
    case StyleHint::ZoomOut:
        return _icons->getIcon("zoom_out");

    default:
        return NULL;
    }
}

Palette*
StylistBase::palette() const
{
    return _palette;
}

void
StylistBase::animate(StyledAnimation type, Widget* target)
{
//    switch (type)
//    {
//    case FocusIn:
//        _focus.targetIn = target;
//        _focus.in->start();
//        break;
//    case FocusOut:
//        _focus.targetOut = target;
//        _focus.out->start();
//        break;
//    default:
//        break;
//    }
}

void
StylistBase::initAnimations()
{
    _focus.in = new TweenAnimation();
    _focus.in->setDuration(500);
    _focus.in->addTween(Tween::SINE, Tween::EASE_OUT, 0, 1);
    _focus.in->sigExec.connect(sigc::bind<StylistBase::StyledAnimation>(sigc::mem_fun(this, &StylistBase::runAnimation), FocusIn));

    _focus.out = new TweenAnimation();
    _focus.out->setDuration(250);
    _focus.out->addTween(Tween::SINE, Tween::EASE_IN, 1, 0);
    _focus.out->sigExec.connect(sigc::bind<StylistBase::StyledAnimation>(sigc::mem_fun(this, &StylistBase::runAnimation), FocusOut));
}

void
StylistBase::runAnimation(StyledAnimation type)
{
    if (type == FocusIn)
        _focus.targetIn->repaint();
    else if (type == FocusOut)
        _focus.targetOut->repaint();
}

} /* namespace ilixi */
