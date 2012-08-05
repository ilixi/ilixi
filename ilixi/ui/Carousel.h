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

#ifndef ILIXI_CAROUSEL_H_
#define ILIXI_CAROUSEL_H_

#include <ui/Widget.h>
#include <lib/TweenAnimation.h>

namespace ilixi
{
class Carousel;
/*!
 * Contains widgets which are added to a Carousel.
 */
class CarouselItem : public Widget
{
public:
    /*!
     * Constructor.
     */
    CarouselItem(Carousel* parent = 0);

    /*!
     * Destructor.
     */
    ~CarouselItem();

    /*!
     * Returns item's position in radians.
     */
    float
    angle() const;

    /*!
     * Sets item's position in radians.
     */
    void
    setAngle(float angle);

    /*!
     * Returns scaling value.
     */
    float
    scale() const;

    /*!
     * Sets value for scaling attached widget.
     */
    void
    setScale(float scale);

    /*!
     * Returns attached widget.
     */
    Widget*
    source() const;

    /*!
     * Attaches given widget to itself.
     */
    void
    setSource(Widget* source);

    /*!
     * This signal is emitted when item receives focus.
     */
    sigc::signal<void> sigFocused;

    /*!
     * This signal is emitted when pointer button is pressed over item.
     */
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
    //! This property stores the attached widget.
    Widget* _source;
    //! This property stores the parent Carousel.
    Carousel* _carousel;
    //! This property stores the angle in radians.
    float _angle;
    //! This property is used to scale dimension of attached widget.
    float _scale;

    void
    updateCarouselItemGeometry();
};

/*!
 * Carousel arranges items around circle.
 */
class Carousel : public Widget
{
public:
    /*!
     * Constructor.
     */
    Carousel(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~Carousel();

    /*!
     * Returns number of items.
     */
    Size
    itemSize() const;

    /*!
     * Adds a CarouselItem and updates.
     */
    void
    addItem(CarouselItem* item);

    /*!
     * Removes a CarouselItem and updates.
     */
    void
    removeItem(CarouselItem* item);

    /*!
     * Adds a new CarouselItem and attaches widget to it.
     */
    void
    addWidget(Widget* widget);

    /*!
     * Removes the CarouselItem which holds given widget.
     */
    void
    removeWidget(Widget* widget);

    /*!
     * Brings the given CarouselItem to front.
     */
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
    //! This property stores the radius on x axis.
    int _radiusX;
    //! This property stores the radius on y axis.
    int _radiusY;
    //! This property stores the center of carousel.
    Point _center;
    //! This property stores the default size for carousel items.
    Size _itemSize;
    //! This property stores the difference in angles between carousel items.
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
