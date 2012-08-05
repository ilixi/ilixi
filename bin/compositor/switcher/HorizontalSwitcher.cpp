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
#include "graphics/Painter.h"

namespace ilixi
{

  HorizontalSwitcher::HorizontalSwitcher(Widget* parent) :
      Switcher(parent)
  {
    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &HorizontalSwitcher::updateSwitcherGeometry));

    _scrollArea = new ScrollArea();
    _scrollArea->setScollerAlways(true, false);
    addChild(_scrollArea);

    _box = new HBoxLayout();
    _box->setSpacing(20);

    _scrollArea->setContent(_box);

    _anim.setDuration(500);
    _anim.sigExec.connect(sigc::mem_fun(this, &HorizontalSwitcher::tweenSlot));
    _anim.sigFinished.connect(
        sigc::mem_fun(this, &HorizontalSwitcher::tweenEndSlot));
    _tween = new Tween(Tween::SINE, Tween::EASE_OUT, 0, 1);
    _anim.addTween(_tween);
  }

  HorizontalSwitcher::~HorizontalSwitcher()
  {
  }

  Size
  HorizontalSwitcher::preferredSize() const
  {
    return _scrollArea->preferredSize();
  }

  void
  HorizontalSwitcher::addThumb(AppThumbnail* thumb)
  {
    if (_box->addWidget(thumb))
      {
        thumb->setVisible(true);
        thumb->setZ(5);
        _thumbs.push_back(thumb);
        thumb->sigFocused.connect(sigc::mem_fun(this, &Switcher::scrollTo));
        thumb->sigSelected.connect(sigSwitchRequest);
        _box->setSize(_box->preferredSize());
        update();
      }
  }

  void
  HorizontalSwitcher::removeThumb(AppThumbnail* thumb)
  {
    _box->removeWidget(thumb);
  }

  void
  HorizontalSwitcher::scrollTo(AppThumbnail* thumb)
  {
    _scrollArea->scrollTo(thumb);
    _current = thumb;
    // TODO WHYYY?
    _scrollArea->scrollTo(thumb);
    _current = thumb;
    // ----
    thumb->setFocus();
    for (unsigned int i = 0; i < _thumbs.size(); ++i)
      if (_thumbs[i] == thumb)
        _currentID = i;
  }

  void
  HorizontalSwitcher::show()
  {
    _anim.stop();
    setVisible(true);
    _tween->setInitialValue(0);
    _tween->setEndValue(1);
    _anim.start();
  }

  void
  HorizontalSwitcher::hide()
  {
    _anim.stop();
    _tween->setInitialValue(1);
    _tween->setEndValue(0);
    _anim.start();
  }

  void
  HorizontalSwitcher::setOptimalGeometry(int width, int height)
  {
    setGeometry(0, height - 196, width, 196);
  }

  void
  HorizontalSwitcher::compose(const PaintEvent& event)
  {
    Painter p(this);
    p.begin(event);
    p.setBrush(Color(0, 0, 0, 80));
    p.fillRectangle(0, height() - (height() * _tween->value()), width(),
        height());
    p.end();
  }

  void
  HorizontalSwitcher::updateSwitcherGeometry()
  {
    _scrollArea->setGeometry(0, 0, width(), height());
  }

  void
  HorizontalSwitcher::tweenSlot()
  {
    _scrollArea->setY(height() - (height() * _tween->value()));
    update();
  }

  void
  HorizontalSwitcher::tweenEndSlot()
  {
    if (_tween->value() < 1)
      setVisible(false);
  }

} /* namespace ilixi */
