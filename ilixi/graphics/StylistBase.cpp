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
        : _palette(NULL),
          _style(NULL)
{
    ILOG_TRACE(ILX_STYLISTBASE);
    _noImage = new Image(ILIXI_DATADIR"images/noImage.png", 48, 48);
    initAnimations();
}

StylistBase::~StylistBase()
{
    delete _palette;
    delete _style;
    delete _noImage;
}

bool
StylistBase::setPaletteFromFile(const char* palette)
{
    return _palette->parsePalette(palette);
}

bool
StylistBase::setStyleFromFile(const char* style)
{
    return _style->parseStyle(style);
}

Size
StylistBase::defaultSize(StyleHint::Size size) const
{
    switch (size)
    {
    case StyleHint::PushButton:
        return Size(defaultParameter(StyleHint::PushButtonCorners),
                    _style->pb.def.m.height());
    case StyleHint::ProgressBar:
        return Size(100, _style->pr.def.m.height());
    case StyleHint::Slider:
        return Size(100, _style->sl.def.m.height());
    case StyleHint::SliderV:
        return Size(_style->sl.defV.m.width(), 100);
    default:
        return Size();
    }
}

int
StylistBase::defaultParameter(StyleHint::Parameter parameter) const
{
    switch (parameter)
    {
    case StyleHint::PushButtonCornerLeft:
        return _style->pb.def.l.width();

    case StyleHint::PushButtonCornerRight:
        return _style->pb.def.r.width();

    case StyleHint::PushButtonCorners:
        return _style->pb.def.l.width() + _style->pb.def.r.width();

    case StyleHint::PushButtonHeight:
        return _style->pb.def.l.height();

    case StyleHint::CheckBoxHeight:
        return _style->cb.def.height();
    case StyleHint::CheckBoxWidth:
        return _style->cb.def.width();

    case StyleHint::RadioHeight:
        return _style->rb.def.height();
    case StyleHint::RadioWidth:
        return _style->rb.def.width();

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
        return _style->tab.def.l.width();
    case StyleHint::TabOffsetRight:
        return _style->tab.def.r.width();
    case StyleHint::TabOffsetTop:
        return 5; //_style->tab.def.tm.height();
    case StyleHint::TabOffsetBottom:
        return _style->tab.def.bm.height();
    case StyleHint::TabOffsetLR:
        return _style->tab.def.l.width() + _style->tab.def.r.width();
    case StyleHint::TabOffsetTB:
        return _style->tab.def.tm.height() + _style->tab.def.bm.height();

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

    default:
        return -1;
    }
}

Font*
StylistBase::defaultFont(StyleHint::Font font) const
{
    switch (font)
    {
    case StyleHint::ButtonFont:
        return _style->_buttonFont;
    case StyleHint::TitleFont:
        return _style->_titleFont;
    case StyleHint::InputFont:
        return _style->_inputFont;
    default:
        return _style->_defaultFont;
    }
}

Image*
StylistBase::defaultIcon(StyleHint::PackedIcon icon) const
{
    ILOG_TRACE(ILX_STYLISTBASE);
    switch (icon)
    {
    case StyleHint::Plus:
        return _style->getIcon("plus");
    case StyleHint::Minus:
        return _style->getIcon("minus");
    case StyleHint::Up:
        return _style->getIcon("arrow_up");
    case StyleHint::Down:
        return _style->getIcon("arrow_down");
    case StyleHint::Left:
        return _style->getIcon("arrow_left");
    case StyleHint::Right:
        return _style->getIcon("arrow_right");
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
    _focus.in->sigExec.connect(
            sigc::bind<StylistBase::StyledAnimation>(
                    sigc::mem_fun(this, &StylistBase::runAnimation), FocusIn));

    _focus.out = new TweenAnimation();
    _focus.out->setDuration(250);
    _focus.out->addTween(Tween::SINE, Tween::EASE_IN, 1, 0);
    _focus.out->sigExec.connect(
            sigc::bind<StylistBase::StyledAnimation>(
                    sigc::mem_fun(this, &StylistBase::runAnimation), FocusOut));
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
