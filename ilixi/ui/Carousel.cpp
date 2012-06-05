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

#include "ui/Carousel.h"
#include "core/Logger.h"
#include "graphics/Painter.h"
#include <sigc++/bind.h>
#include <cmath>

namespace ilixi
{

  const double PI = 3.141592;
  const double PI_TWICE = PI * 2;
  const double PI_HALF = PI / 2.0;

  CarouselItem::CarouselItem(Carousel* parent) :
      Widget(parent), _source(NULL), _carousel(parent), _angle(0), _scale(0)
  {
    setInputMethod(KeyAndPointerInput);
    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &CarouselItem::updateCarouselItemGeometry));
  }

  CarouselItem::~CarouselItem()
  {
  }

  float
  CarouselItem::angle() const
  {
    return _angle;
  }

  void
  CarouselItem::setAngle(float angle)
  {
    if (angle > (PI_TWICE))
      angle = angle - (PI_TWICE);
    else if (angle < 0)
      angle = (PI_TWICE) + angle;
    _angle = angle;
  }

  float
  CarouselItem::scale() const
  {
    return _scale;
  }

  void
  CarouselItem::setScale(float scale)
  {
    _scale = scale;
    setZ(-15 + _scale * 20);
    setSize(_carousel->itemSize().width() * _scale,
        _carousel->itemSize().height() * _scale);
  }

  Widget*
  CarouselItem::source() const
  {
    return _source;
  }

  void
  CarouselItem::setSource(Widget* source)
  {
    _source = source;
    _source->setInputMethod(NoInput);
    addChild(_source);
  }

  void
  CarouselItem::compose(const PaintEvent& event)
  {
    Painter p(this);
    p.begin(event);
    if (state() & FocusedState)
      p.setBrush(Color(128, 128, 255));
    else if (state() & ExposedState)
      p.setBrush(Color(128, 0, 255));
    else
      p.setBrush(Color(255, 255, 255));
    p.fillRectangle(0, 0, width(), height());
    p.end();
  }

  void
  CarouselItem::focusInEvent()
  {
    sigFocused();
  }

  void
  CarouselItem::enterEvent(const PointerEvent& event)
  {
    update();
  }

  void
  CarouselItem::leaveEvent(const PointerEvent& event)
  {
    update();
  }

  void
  CarouselItem::keyUpEvent(const KeyEvent& event)
  {
    if (event.keySymbol == DIKS_SPACE || event.keySymbol == DIKS_OK)
      sigPressed();
  }

  void
  CarouselItem::updateCarouselItemGeometry()
  {
    if (_source)
      _source->setGeometry(5, 5, width() - 10, height() - 10);
  }

  //****************************************************************************

  Carousel::Carousel(Widget* parent) :
      Widget(parent)
  {
    sigGeometryUpdated.connect(
        sigc::mem_fun(this, &Carousel::updateCarouselGeometry));
    setInputMethod(PointerInput);
    _animation.setDuration(500);
    _tween = new Tween(Tween::SINE, Tween::EASE_OUT, 0, 1);
    _animation.addTween(_tween);
    _animation.sigExec.connect(sigc::mem_fun(this, &Carousel::tweenSlot));
    _animation.sigFinished.connect(
        sigc::mem_fun(this, &Carousel::tweenEndSlot));
  }

  Carousel::~Carousel()
  {
  }

  Size
  Carousel::itemSize() const
  {
    return _itemSize;
  }

  void
  Carousel::addItem(Widget* widget)
  {
    CarouselItem* item = new CarouselItem(this);
    addChild(item);
    item->setSource(widget);
    item->sigFocused.connect(
        sigc::bind<CarouselItem*>(sigc::mem_fun(this, &Carousel::showItem),
            item));
    updateCarouselGeometry();
  }

  void
  Carousel::removeItem(Widget* widget)
  {
    removeChild(widget);
  }

  void
  Carousel::showItem(CarouselItem* widget)
  {
    for (WidgetListIterator it = _children.begin(); it != _children.end(); ++it)
      {
        if (((CarouselItem*) *it) == widget)
          {
            float target = PI_HALF - widget->angle();
            _animation.stop();
            _tween->setEndValue(target);
            _animation.start();
            break;
          }
      }
  }

  void
  Carousel::compose(const PaintEvent& event)
  {
  }

  void
  Carousel::tweenSlot()
  {
    for (WidgetListIterator it = _children.begin(); it != _children.end(); ++it)
      {

        CarouselItem* item = dynamic_cast<CarouselItem*>(*it);
        if (!item)
          continue;

        float angle = item->angle() + _tween->value();

        item->setY(std::sin(angle) * _radiusY + _center.y());

        item->setScale(1.0 * item->y() / (_center.y() + _radiusY));

        item->setX(
            std::cos(angle) * _radiusX + _center.x() - item->width() / 2);
      }

    update();
  }

  void
  Carousel::tweenEndSlot()
  {
    CarouselItem* selected;
    int i = 0;
    for (WidgetListIterator it = _children.begin(); it != _children.end();
        ++it, ++i)
      {

        CarouselItem* item = dynamic_cast<CarouselItem*>(*it);
        if (!item)
          continue;
        item->setAngle(item->angle() + _tween->endValue());
        if (item->scale() == 1)
          selected = item;
        ILOG_DEBUG( ILX_CAROUSEL,
            "%d z%d "
            "angle: %f "
            "scale %f - "
            "%d, %d, %d, %d\n", i, item->z(), item->angle(), item->scale(), item->x(), item->y(), item->width(), item->height());
      }
    sigItemSelected(selected);
  }

  void
  Carousel::updateCarouselGeometry()
  {
    if (_children.empty())
      return;

    _angleStep = (PI_TWICE) / _children.size();

    int i = 0;
    int xUnit = width() / 9.0;
    int yUnit = height() / 7.0;

    _radiusX = xUnit * 3.5;
    _radiusY = yUnit * 1.5;
    _center.setX(width() / 2);
    _center.setY(height() / 2 - yUnit);

    _itemSize = Size(xUnit * 3, yUnit * 3);

    ILOG_DEBUG( ILX_CAROUSEL,
        "AngleStep: %f Radius(%d, %d) Center(%d, %d)\n", _angleStep, _radiusX, _radiusY, _center.x(), _center.y());

    float angle = PI_HALF;

    Widget* right = getNeighbour(Right);
    Widget* first = NULL;
    for (WidgetListIterator it = _children.begin(); it != _children.end();
        ++it, ++i)
      {

        CarouselItem* item = dynamic_cast<CarouselItem*>(*it);
        if (!item)
          continue;

        item->setAngle(angle);

        item->setY(_center.y() + std::sin(angle) * _radiusY);

        item->setScale(1.0 * item->y() / (_center.y() + _radiusY));

        item->setX(
            std::cos(angle) * _radiusX + _center.x() - item->width() / 2);

        if (right)
          right->setNeighbour(Right, item);
        item->setNeighbour(Right, right);

        ILOG_DEBUG( ILX_CAROUSEL,
            "%d z%d "
            "angle: %f "
            "scale %f - "
            "%d, %d, %d, %d\n", i, item->z(), item->angle(), item->scale(), item->x(), item->y(), item->width(), item->height());

        angle += _angleStep;
        right = item;
      }
  }

} /* namespace ilixi */
