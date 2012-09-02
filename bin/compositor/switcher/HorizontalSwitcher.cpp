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

#include "HorizontalSwitcher.h"
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_HSWITCHER, "ilixi/comp/HSwitcher", "HorizontalSwitcher");

HorizontalSwitcher::HorizontalSwitcher(Widget* parent)
        : Switcher(parent)
{
    ILOG_TRACE_W(ILX_HSWITCHER);
    sigGeometryUpdated.connect(
            sigc::mem_fun(this, &HorizontalSwitcher::updateSwitcherGeometry));

    _scroller = new HorizontalScroller();
    addChild(_scroller);

    _anim.setDuration(500);
    _anim.sigExec.connect(sigc::mem_fun(this, &HorizontalSwitcher::tweenSlot));
    _anim.sigFinished.connect(
            sigc::mem_fun(this, &HorizontalSwitcher::tweenEndSlot));
    _tween = new Tween(Tween::QUAD, Tween::EASE_OUT, 0, 1);
    _anim.addTween(_tween);

    _bg = new Image(ILIXI_DATADIR"compositor/switcher.png");

    _left = new ToolButton("left");
    _left->setDrawFrame(false);
    _left->setToolButtonStyle(ToolButton::IconOnly);
    _left->setIcon(new Icon(ILIXI_DATADIR"compositor/left.png"));
    _left->sigClicked.connect(
            sigc::mem_fun(this, &HorizontalSwitcher::scrollToPrevious));
    addChild(_left);
    _left->setVisible(false);

    _right = new ToolButton("right");
    _right->setDrawFrame(false);
    _right->setToolButtonStyle(ToolButton::IconOnly);
    _right->setIcon(new Icon(ILIXI_DATADIR"compositor/right.png"));
    _right->sigClicked.connect(
            sigc::mem_fun(this, &HorizontalSwitcher::scrollToNext));
    addChild(_right);
    _right->setVisible(false);

    _scroller->setOffset(Point(100, 0));
}

HorizontalSwitcher::~HorizontalSwitcher()
{
    ILOG_TRACE_W(ILX_HSWITCHER);
    delete _bg;
}

Size
HorizontalSwitcher::preferredSize() const
{
    ILOG_TRACE_W(ILX_HSWITCHER);
    return _scroller->preferredSize();
}

void
HorizontalSwitcher::addThumb(AppThumbnail* thumb)
{
    ILOG_TRACE_W(ILX_HSWITCHER);
    if (_scroller->addWidget(thumb))
    {
        if (_scroller->_selectedWidget == NULL)
            _scroller->_selectedWidget = thumb;

        _scroller->setSize(_scroller->preferredSize());
        _thumbs.push_back(thumb);
        thumb->setVisible(true);
        thumb->sigSelected.connect(sigSwitchRequest);

        if (_thumbs.size() > _num2Slide)
        {
            _left->setVisible(true);
            _right->setVisible(true);
        }

        update();
    }
}

void
HorizontalSwitcher::removeThumb(AppThumbnail* thumb)
{
    ILOG_TRACE_W(ILX_HSWITCHER);
    _scroller->removeWidget(thumb);

    for (Thumbnails::iterator it = _thumbs.begin(); it != _thumbs.end(); ++it)
    {
        if (*it == thumb)
        {
            _thumbs.erase(it);
            break;
        }
    }

    if (_thumbs.size() == _num2Slide)
    {
        _currentIndex = 0;
        scrollTo(currentThumb());
        _left->setVisible(false);
        _right->setVisible(false);
    }
}

void
HorizontalSwitcher::scrollTo(AppThumbnail* thumb)
{
    ILOG_TRACE_W(ILX_HSWITCHER);
    ILOG_DEBUG(ILX_HSWITCHER, " -> thumb: %p\n", thumb);
    setCurrentThumb(thumb);
    _scroller->scrollTo(thumb);
}

void
HorizontalSwitcher::show()
{
    ILOG_TRACE_W(ILX_HSWITCHER);
    _anim.stop();
    setVisible(true);
    _tween->setInitialValue(height());
    _tween->setEndValue(0);
    _anim.start();
}

void
HorizontalSwitcher::hide()
{
    ILOG_TRACE_W(ILX_HSWITCHER);
    _anim.stop();
    _tween->setInitialValue(0);
    _tween->setEndValue(height());
    _anim.start();
}

void
HorizontalSwitcher::scrollToNext()
{
    ILOG_TRACE_W(ILX_HSWITCHER);
    if (_thumbs.size() == 0)
        return;

    _currentIndex += _num2Slide;
    if (_currentIndex > _thumbs.size() - 1)
        _currentIndex = 0;

    scrollTo(currentThumb());
}

void
HorizontalSwitcher::scrollToPrevious()
{
    ILOG_TRACE_W(ILX_HSWITCHER);
    if (_thumbs.size() == 0)
        return;

    _currentIndex -= _num2Slide;
    if (_currentIndex < 0)
        _currentIndex = _thumbs.size() - _thumbs.size() % _num2Slide;

    scrollTo(currentThumb());
}

void
HorizontalSwitcher::setOptimalGeometry(int w, int h)
{
    ILOG_TRACE_W(ILX_HSWITCHER);
    _anchorY = h - 196;
    setGeometry(0, _anchorY, w, 196);
    _num2Slide = (width() - 200) / 196.0;
}

void
HorizontalSwitcher::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    p.stretchImage(_bg, Rectangle(0, 0, width(), height()));
    p.end();
}

void
HorizontalSwitcher::updateSwitcherGeometry()
{
    ILOG_TRACE_W(ILX_HSWITCHER);
    _scroller->setGeometry(0, 20, width(), height() - 40);
    _left->setGeometry(0, 0, 100, 200);
    _right->setGeometry(width() - 100, 0, 100, 200);
}

void
HorizontalSwitcher::tweenSlot()
{
    setY(_anchorY + _tween->value());
    update();
}

void
HorizontalSwitcher::tweenEndSlot()
{
    if (_tween->value() > 0)
        setVisible(false);
}

} /* namespace ilixi */
