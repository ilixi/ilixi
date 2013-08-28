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

#ifndef ILIXI_SPINBOX_H_
#define ILIXI_SPINBOX_H_

#include <ui/DirectionalButton.h>

namespace ilixi
{
//! Provides a spin box widget.
/*!
 * SpinBox allows to choose an integer value using plus and minus buttons.
 */
class SpinBox : public Widget
{
public:
    /*!
     * Constructor.
     * @param value initial value.
     * @param parent
     */
    SpinBox(int value, Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~SpinBox();

    virtual Size
    preferredSize() const;

    /*!
     * Returns maximum value.
     */
    int
    max() const;

    /*!
     * Returns minimum value.
     */
    int
    min() const;

    /*!
     * Returns step size.
     */
    int
    step() const;

    /*!
     * Returns current value.
     */
    int
    value() const;

    /*!
     * Returns text appended to the start of text.
     */
    const std::string&
    postfix() const;

    /*!
     * Returns text prepended to the start of text.
     */
    const std::string&
    prefix() const;

    /*!
     * Returns whether wrapping is enabled.
     */
    bool
    wrapping() const;

    /*!
     * Returns text layout.
     */
    TextLayout
    layout() const;

    /*!
     * Sets maximum value.
     */
    void
    setMax(int max);

    /*!
     * Sets minimum value.
     */
    void
    setMin(int min);

    /*!
     * Sets minimum and maximum values.
     */
    void
    setRange(int min, int max);

    /*!
     * Sets step size.
     */
    void
    setStep(int step);

    /*!
     * Sets current value.
     *
     * This method will emit sigValueChanged.
     */
    void
    setValue(int value);

    /*!
     * Sets whether wrapping is enabled or not.
     */
    void
    setWrapping(bool wrapping);

    /*!
     * Sets text appended at the end.
     */
    void
    setPostfix(const std::string& postfix);

    /*!
     * Sets text prepended at the start.
     */
    void
    setPrefix(const std::string& prefix);

    /*!
     * This signal is emitted when spinbox's value is changed.
     */
    sigc::signal<void, int> sigValueChanged;

protected:
    void
    compose(const PaintEvent& event);

private:
    //! Button for increments.
    DirectionalButton* _plus;
    //! Button for decrements.
    DirectionalButton* _minus;

    //! Prefix for input.
    std::string _prefix;
    //! Postfix for input.
    std::string _postfix;
    //! TextLayout for this widget.
    TextLayout _layout;

    //! This property stores the minimum value.
    int _min;
    //! This property stores the maximum value.
    int _max;
    //! This property stores the current value.
    int _value;
    //! This property stores the increment/decrement amount.
    int _step;
    //! This property stores whether spinbox wraps at ends.
    bool _wrapping;

    void
    decrement();

    void
    increment();

    void
    updateSpinBoxGeometry();
};

} /* namespace ilixi */
#endif /* ILIXI_SPINBOX_H_ */
