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

#ifndef ILIXI_CAROUSEL_H_
#define ILIXI_CAROUSEL_H_

#include "ui/Widget.h"
#include "lib/TweenAnimation.h"

namespace ilixi
{
  class Carousel;
  class CarouselItem : public Widget
  {
  public:
    CarouselItem(Carousel* parent = 0);

    ~CarouselItem();

    float
    angle() const;

    void
    setAngle(float angle);

    float
    scale() const;

    void
    setScale(float scale);

    Widget*
    source() const;

    void
    setSource(Widget* source);

    sigc::signal<void> sigFocused;
    sigc::signal<void> sigPressed;

  protected:
    void
    compose(const PaintEvent& event);

    virtual void
    focusInEvent();

    virtual void
    enterEvent(const PointerEvent& event);

    virtual void
    leaveEvent(const PointerEvent& event);

    virtual void
    keyUpEvent(const KeyEvent& event);

  private:
    Widget* _source;
    Carousel* _carousel;
    float _angle;
    float _scale;

    void
    updateCarouselItemGeometry();
  };

  class Carousel : public Widget
  {
  public:
    Carousel(Widget* parent = 0);

    virtual
    ~Carousel();

    Size
    itemSize() const;

    void
    addItem(Widget* widget);

    void
    removeItem(Widget* widget);

    void
    showItem(CarouselItem* item);

    /*!
     * This signal is emitted once an item is selected.
     */
    sigc::signal<void, CarouselItem*> sigItemSelected;

  protected:
    void
    compose(const PaintEvent& event);

  private:
    int _radiusX;
    int _radiusY;
    Point _center;
    Size _itemSize;
    float _angleStep;

    TweenAnimation _animation;
    Tween* _tween;

    void
    tweenSlot();

    void
    tweenEndSlot();

    void
    updateCarouselGeometry();
  };

} /* namespace ilixi */
#endif /* ILIXI_CAROUSEL_H_ */
