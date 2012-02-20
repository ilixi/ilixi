/*
 Copyright 2011 Tarik Sekmen.

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

#include "ui/ScrollArea.h"
#include "graphics/Painter.h"
#include "lib/TweenAnimation.h"
#include <cmath>

using namespace ilixi;

// maximum number of pointer events in queue.
const int queueSize = 41;
// this variable specifies how fast content bounces back when it is outside edges (greater = faster bounce).
const int rubber = 35;
// this variable specifies how much velocity is reduced in each update (greater = faster decrease).
const float friction = 0.02;

ScrollArea::ScrollArea(Widget* parent) :
    Widget(parent), _options(0x009), _content(NULL), _aniVal(0), _cx(0), _cy(0), _vx(
        0), _vy(0)
{
  setInputMethod(PointerInputTracking);
  setConstraints(NoConstraint, NoConstraint);
  sigGeometryUpdated.connect(
      sigc::mem_fun(this, &ScrollArea::updateScollAreaGeometry));

  _ani = new TweenAnimation();
  _ani->addTween(Tween::CUBIC, Tween::EASE_OUT, _aniVal, 1, 0);
  _ani->setDuration(1000);
  _ani->sigStep.connect(sigc::mem_fun(this, &ScrollArea::updateScrollArea));
}

ScrollArea::~ScrollArea()
{
  delete _ani;
}

Size
ScrollArea::preferredSize() const
{
  if (_content)
    return _content->preferredSize();
  return Size(100, 100);
}

void
ScrollArea::setContent(Widget* content)
{
  if (content)
    {
      removeChild(_content);
      _content = content;
      if (_options & SmoothScrolling)
        _content->_surfaceDesc = BlitDescription;
      addChild(_content);
      raiseChildToFront(_content);
    }
}

void
ScrollArea::setScollerAlways(bool horizontal, bool vertical)
{
  if (horizontal)
    _options &= ~HorizontalAuto;
  if (vertical)
    _options &= ~VerticalAuto;
}

void
ScrollArea::setAutoScrolling(bool autoScroll)
{
  if (autoScroll)
    _options |= HorizontalAuto | VerticalAuto;
  else
    _options |= HorizontalAuto | VerticalAuto;
}

void
ScrollArea::setSmoothScrolling(bool smoothScroll)
{
  if (smoothScroll)
    {
      _options |= SmoothScrolling;
      _content->_surfaceDesc = BlitDescription;
    }
  else
    {
      _options &= ~SmoothScrolling;
      _content->_surfaceDesc = DefaultDescription;
    }
}

void
ScrollArea::scrollTo(int x, int y)
{
  // TODO finish scrollTo method.
  _ani->stop();
  int deltaX = -x == _cx ? 0 : -x < _cx ? _cx + x : -x;
  int deltaY = y < -_cy ? y : 0;
  _sCur = Point(deltaX, _cy - y);
  _options |= TargetedScroll;
  _ani->start();
}

void
ScrollArea::paint(const Rectangle& rect)
{
  if (visible())
    {
      updateSurface();
      Rectangle intersect = _frameGeometry.intersected(rect);
      if (intersect.isValid())
        {
          if (_options & SmoothScrolling)
            {
              _content->moveTo(_cx, _cy);
              if (_content->surface())
                _content->surface()->clear();
              _content->paint(
                  Rectangle(0, 0, _content->width(), _content->height()));
              _content->surface()->flip();
              surface()->blit(_content->surface(),
                  Rectangle(-_cx, -_cy, width(), height()), 0, 0);
            }
          else
            {
              _content->moveTo(_cx, _cy);
              paintChildren(intersect);
            }
          compose();
        }
    }
}

void
ScrollArea::pointerGrabEvent(const PointerEvent& event)
{
  _sCur = Point(event.x, event.y);
  _sPre = _sCur;
  _ani->stop();
  _aniVal = 1;
  updateScrollArea(1);
  _options &= ~TargetedScroll;
}

void
ScrollArea::pointerReleaseEvent(const PointerEvent& event)
{
  _vx = 0;
  _vy = 0;
  if (_events.size() > 1)
    {
      int deltaX = 0;
      int deltaY = 0;
      int deltaT = 0;
      float k = 1.0 / _events.size();
      float w = k;
      float wT = 0;
      PointerEvent cur;
      PointerEvent pre;
      pre = _events.front();
      _events.pop();
      while (!_events.empty())
        {
          cur = _events.front();
          deltaX = cur.x - pre.x;
          deltaY = cur.y - pre.y;
          deltaT = cur.timestamp - pre.timestamp;
          _vx += deltaX == 0 ? 0 : deltaT > 0 ? w * deltaX / deltaT : 0;
          _vy += deltaY == 0 ? 0 : deltaT > 0 ? w * deltaY / deltaT : 0;
          pre = cur;
          w += k;
          wT += w;
          _events.pop();
        }
      _vx /= wT;
      _vy /= wT;
    }
  _options &= ~TargetedScroll;
  _ani->start();
}

void
ScrollArea::pointerMotionEvent(const PointerEvent& event)
{
  if (pressed())
    {
      _sCur = Point(event.x, event.y);
      _events.push(event);
      if (_events.size() == queueSize)
        _events.pop();
      _aniVal = 1;
      updateScrollArea(1);
    }
}

void
ScrollArea::pointerWheelEvent(const PointerEvent& event)
{
  _ani->stop();
  _cy += height() / 10 * event.wheelStep;
  _ani->start();
}

void
ScrollArea::compose()
{
  Painter p(this);
  p.begin();
  p.drawRectangle(0, 0, width(), height());
  if (_aniVal)
    {
      p.setBrush(Color(0, 0, 0, _aniVal * 255));
      if (_options & HorizontalScroll)
        {
          int y = _thumbs.y();
          int h = 10;
          int w = _thumbs.width();
          int x = 1 + (width() - w - 12.0) * _cx / _sMax.x();
          p.fillRectangle(x, y, w, h);
        }

      if (_options & VerticalScroll)
        {
          int x = _thumbs.x();
          int w = 10;
          int h = _thumbs.height();
          int y = 1 + (height() - h - 12.0) * _cy / _sMax.y();
          p.fillRectangle(x, y, w, h);
        }
    }
  p.end();
}

void
ScrollArea::updateScollAreaGeometry()
{
  _content->setSize(_content->preferredSize());
  _sMax = Rectangle(width() - _content->width(), height() - _content->height(),
      width() / 3, height() / 3);
  _thumbs = Rectangle(width() - 11, height() - 11,
      width() * width() / _content->width() - 4,
      height() * height() / _content->height() - 4);

  if (_options & HorizontalAlways)
    _options |= HorizontalScroll;
  else if (_options & HorizontalAuto && _content->width() > width())
    _options |= HorizontalScroll;
  else
    _options &= ~HorizontalScroll;

  if (_content->width() < width() && _content->xConstraint() & GrowPolicy)
    _content->setWidth(width());

  if (_options & VerticalAlways)
    _options |= VerticalScroll;
  else if (_options & VerticalAuto && _content->height() > height())
    _options |= VerticalScroll;
  else
    _options &= ~VerticalScroll;

  if (_content->height() < height() && _content->yConstraint() & GrowPolicy)
    _content->setHeight(height());
}

void
ScrollArea::updateScrollArea(int step)
{
  if (pressed())
    {
      if (_options & HorizontalScroll)
        {
          float deltaX = _sCur.x() - _sPre.x();

          if (_cx > 0)
            deltaX *= 1.0 - (_cx / _sMax.width());
          else if (_cx < _sMax.x())
            deltaX *= 1.0 - ((_sMax.x() - _cx) / _sMax.width());

          _cx += deltaX;
        }

      if (_options & VerticalScroll)
        {
          float deltaY = _sCur.y() - _sPre.y();

          if (_cy > 0)
            deltaY *= 1.0 - (_cy / _sMax.height());
          else if (_cy < _sMax.y())
            deltaY *= 1.0 - ((_sMax.y() - _cy) / _sMax.height());

          _cy += deltaY;
        }

      _sPre = _sCur;
    }
  else if (_options & TargetedScroll)
    {
      if (_options & HorizontalScroll)
        _cx = (1 - _aniVal) * _sCur.x();
      if (_options & VerticalScroll)
        _cy = (1 - _aniVal) * _sCur.y();
    }
  else
    {
      if (_options & HorizontalScroll)
        {
          if (_cx >= 0)
            _vx = -(1 - _aniVal) * (_cx) / rubber;
          else if (_cx <= _sMax.x())
            _vx = (1 - _aniVal) * (_sMax.x() - _cx) / rubber;
          else if (fabs(_vx) > friction)
            _vx -= _vx * friction;
          else
            _vx = 0;
          _cx += _vx * step;
        }

      if (_options & VerticalScroll)
        {
          if (_cy >= 0)
            _vy = -(1 - _aniVal) * (_cy) / rubber;
          else if (_cy <= _sMax.y())
            _vy = (1 - _aniVal) * (_sMax.y() - _cy) / rubber;
          else if (fabs(_vy) > friction)
            _vy -= _vy * friction;
          else
            _vy = 0;
          _cy += _vy * step;
        }
    }

  if (_options & SmoothScrolling)
    {
      _content->moveTo(_cx, _cy);
      surface()->clear();
      surface()->blit(_content->surface(),
          Rectangle(-_cx, -_cy, width(), height()), 0, 0);
      compose();
      surface()->flip();
    }
  else
    repaint();
}
