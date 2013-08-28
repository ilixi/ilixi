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

#ifndef ILIXI_SCROLLBAR_H_
#define ILIXI_SCROLLBAR_H_

#include <ui/DirectionalButton.h>

namespace ilixi
{

class ScrollBar : public Widget
{
public:
    ScrollBar(Orientation orientation = Horizontal, Widget* parent = 0);

    virtual
    ~ScrollBar();

    Size
    preferredSize() const;

    const Rectangle&
    indicator() const;

    const Rectangle&
    indicatorRegion() const;

    Direction
    direction() const;

    int
    max() const;

    int
    min() const;

    Orientation
    orientation() const;

    int
    step() const;

    int
    value() const;

    void
    setDirection(Direction direction);

    void
    setMax(int max);

    void
    setMin(int min);

    void
    setOrientation(Orientation orientation);

    void
    setRange(int min, int max);

    void
    setStep(int step);

    void
    setValue(int value);

    void
    scroll(int amount);

    /*!
     * This signal is emitted when pointer button is pressed over scroll bar.
     */
    sigc::signal<void> sigPressed;

    /*!
     * This signal is emitted when pointer button is released over scroll bar.
     */
    sigc::signal<void> sigReleased;

    /*!
     * This signal is emitted when pointer is released and scroll bar value is changed.
     */
    sigc::signal<void, int> sigValueChanged;

protected:
    virtual void
    compose(const PaintEvent& event);

    virtual void
    pointerButtonDownEvent(const PointerEvent& pointerEvent);

    virtual void
    pointerButtonUpEvent(const PointerEvent& pointerEvent);

    virtual void
    pointerMotionEvent(const PointerEvent& pointerEvent);

    virtual void
    pointerWheelEvent(const PointerEvent& event);

private:
    //! This property stores minimum limit.
    int _min;
    //! This property stores maximum limit.
    int _max;
    //! This property stores current value.
    int _value;
    //! This property stores step amount in both directions, e.g. button press.
    int _step;
    //! This property stores the placement of bar in parent container.
    Direction _direction;
    //! This property stores the orientation of scroll bar.
    Orientation _orientation;
    //! This property stores indicator rectangle which receives pointer events.
    Rectangle _indicator;
    //! This property stores a rectangle where indicator moves.
    Rectangle _indicatorRegion;
    //! This property stores the pointer input translation rectangle.
    Rectangle _pRect;
    //! Button for decrementing value.
    DirectionalButton* _dec;
    //! Button for incrementing value.
    DirectionalButton* _inc;

    void
    setSBIndicator(const Point& point);

    //! Updates indicator rectangle using current value.
    void
    updateIndicatorGeometry();

    //! Scroll up or left.
    void
    scrollDec();

    //! Scroll down or right.
    void
    scrollInc();

    //! Updates scroll bar geometry.
    void
    updateSBGeometry();
};

} /* namespace ilixi */
#endif /* ILIXI_SCROLLBAR_H_ */
