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

#ifndef ILIXI_SLIDER_H_
#define ILIXI_SLIDER_H_

#include <ui/Widget.h>

namespace ilixi
{

//! A slider input widget.
class Slider : public Widget
{
    friend class Stylist;
public:
    /*!
     * Constructor creates a horizontal slider.
     * Sets min=0, max=100, step=1, pageStep=10.
     */
    Slider(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~Slider();

    Size
    preferredSize() const;

    bool
    inverted() const;

    float
    maximum() const;

    float
    minimum() const;

    Orientation
    orientation() const;

    float
    pageStep() const;

    float
    step() const;

    float
    range() const;

    /*!
     * Returns slider's value.
     */
    float
    value() const;

    void
    setInverted(bool inverted);

    /*!
     * Sets slider's value.
     */
    void
    setValue(float value, bool signal = true);

    /*!
     * Sets slider's minimum and maximum limits.
     */
    void
    setRange(float minimum, float maximum);

    /*!
     * Sets slider's step amount. This value specifies how much slider is moved when an arrow key is pressed or mouse wheel is rotated.
     * If step is 0 then slider will not respond to arrow keys or mouse wheel.
     *
     * @param step This value is usually smaller than pageStep amount.
     */
    void
    setStep(float step);

    /*!
     *  Sets slider's page step amount.  This value specifies how much slider is moved when a PageUp or PageDown key is pressed.
     *  If pageStep is 0 then slider will not respond to Page keys.
     *
     * @param pageStep This value is usually larger than step amount.
     */
    void
    setPageStep(float pageStep);

    void
    setMaximum(float maximum);

    void
    setMinimum(float minimum);

    void
    setOrientation(Orientation orientation);

    /*!
     * This signal is emitted when pointer button is pressed over slider.
     */
    sigc::signal<void> sigPressed;

    /*!
     * This signal is emitted when pointer button is released over slider.
     */
    sigc::signal<void> sigReleased;

    /*!
     * This signal is emitted when pointer is released and slider's value is changed.
     */
    sigc::signal<void, float> sigValueChanged;

protected:
    //! This property holds slider's maximum limit.
    float _maximum;
    //! This property holds slider's minimum limit.
    float _minimum;
    //! This property holds the current value of slider.
    float _value;
    //! This property holds the step amount of slider.
    float _step;
    //! This property holds the page step amount of slider.
    float _pageStep;
    //! This property holds the orientation of the progress bar.
    Orientation _orientation;
    //! This property holds the bounding rectangle around indicator.
    Point _indicator;
    //! This is the difference between _maximum and _minimum. This is stored in order to simplify calculations.
    float _range;
    //! This flag specifies whether slider is inverted.
    bool _inverted;

    virtual void
    compose(const PaintEvent& event);

    virtual void
    keyDownEvent(const KeyEvent& keyEvent);

    virtual void
    pointerButtonDownEvent(const PointerEvent& pointerEvent);

    virtual void
    pointerButtonUpEvent(const PointerEvent& pointerEvent);

    virtual void
    pointerMotionEvent(const PointerEvent& pointerEvent);

    virtual void
    pointerWheelEvent(const PointerEvent& event);

    virtual void
    enterEvent(const PointerEvent& event);

    virtual void
    leaveEvent(const PointerEvent& event);

    virtual void
    focusInEvent();

    virtual void
    focusOutEvent();

private:
    //! Calculates slider value using given point.
    void
    setValueUsingPoint(const Point& p);

    //! Updates indicator geometry.
    void
    updateIndicatorPosition();
};

} /* namespace ilixi */
#endif /* ILIXI_SLIDER_H_ */
