/*
 Copyright 2012 Tarik Sekmen.

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

#include "Switcher.h"
#include "ui/PushButton.h"
#include "graphics/Painter.h"

namespace ilixi
{

  Switcher::Switcher(Widget* parent) :
      Widget(parent), _current(NULL), _currentID(-1)
  {
    setInputMethod(PointerInput);
    setConstraints(ExpandingConstraint, FixedConstraint);

    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &Switcher::updateSwitcherGeometry));

    _scrollArea = new ScrollArea();
    _scrollArea->setScollerAlways(true, false);
    addChild(_scrollArea);

    _box = new HBoxLayout();
    _box->setSpacing(20);

    _scrollArea->setContent(_box);

    _anim.setDuration(500);
    _anim.sigExec.connect(sigc::mem_fun(this, &Switcher::tweenSlot));
    _anim.sigFinished.connect(sigc::mem_fun(this, &Switcher::tweenEndSlot));
    _tween = new Tween(Tween::SINE, Tween::EASE_OUT, 0, 1);
    _anim.addTween(_tween);

    setVisible(false);
  }

  Switcher::~Switcher()
  {
  }

  Size
  Switcher::preferredSize() const
  {
    return _scrollArea->preferredSize();
  }

  void
  Switcher::scrollTo(AppThumbnail* thumb)
  {
    if (_current)
      _current->hideOverlay();
    _scrollArea->scrollTo(thumb);
    _current = thumb;
    _current->showOverlay();
    _scrollArea->scrollTo(thumb);
    _current = thumb;
    thumb->setFocus();
    for (unsigned int i = 0; i < _thumbs.size(); ++i)
      if (_thumbs[i] == thumb)
        _currentID = i;
  }

  void
  Switcher::scrollToNext()
  {
    scrollTo(nextThumb());
  }

  void
  Switcher::scrollToPrevious()
  {
    scrollTo(previousThumb());
  }

  AppThumbnail*
  Switcher::currentThumb()
  {
    return _current;
  }

  AppThumbnail*
  Switcher::nextThumb()
  {
    if (_currentID >= 0)
      {
        if (++_currentID == _thumbs.size())
          _currentID = 0;

        return _thumbs[_currentID];
      }
    return NULL;
  }

  AppThumbnail*
  Switcher::previousThumb()
  {
    if (_currentID >= 0)
      {
        if (--_currentID == -1)
          _currentID = _thumbs.size() - 1;

        return _thumbs[_currentID];
      }
    return NULL;
  }

  void
  Switcher::addThumb(AppThumbnail* thumb)
  {
    if (_box->addWidget(thumb))
      {
        thumb->setVisible(true);
        _thumbs.push_back(thumb);
        thumb->sigFocused.connect(sigc::mem_fun(this, &Switcher::scrollTo));
        thumb->sigSelected.connect(sigc::mem_fun(this, &Switcher::viewRequest));
        _box->setSize(_box->preferredSize());
        update();
      }
  }

  void
  Switcher::removeThumb(AppThumbnail* thumb)
  {
    _box->removeWidget(thumb);
  }

  void
  Switcher::show()
  {
    _anim.stop();
    setVisible(true);
    _tween->setInitialValue(0);
    _tween->setEndValue(1);
    _anim.start();
  }

  void
  Switcher::hide()
  {
    _anim.stop();
    _tween->setInitialValue(1);
    _tween->setEndValue(0);
    _anim.start();
  }

  void
  Switcher::compose(const Rectangle& rect)
  {
    Painter p(this);
    p.begin(rect);
    p.setBrush(Color(0, 0, 0, 80));
    p.fillRectangle(0, height() - (height() * _tween->value()), width(),
        height());
    p.end();
  }

  void
  Switcher::updateSwitcherGeometry()
  {
    _scrollArea->setGeometry(0, 0, width(), height());
  }

  void
  Switcher::tweenSlot()
  {
    _scrollArea->setY(height() - (height() * _tween->value()));
    update();
  }

  void
  Switcher::tweenEndSlot()
  {
    if (_tween->value() < 1)
      setVisible(false);
  }

  void
  Switcher::viewRequest()
  {
    sigSwitchRequest();
  }

} /* namespace ilixi */
