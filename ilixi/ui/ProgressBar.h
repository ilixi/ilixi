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

#ifndef ILIXI_PROGRESSBAR_H_
#define ILIXI_PROGRESSBAR_H_

#include <ui/Widget.h>

namespace ilixi
{
//! A horizontal or vertical progress bar.
class ProgressBar : public Widget
{
public:
    /*!
     * Constructor.
     * @param parent
     */
    ProgressBar(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~ProgressBar();

    virtual Size
    preferredSize() const;

    /*!
     *
     * @return
     */
    Orientation
    orientation() const;

    /*!
     * Returns progress bar's maximum value limit.
     */
    int
    maximum() const;

    /*!
     * Returns progress bar's minimum value limit.
     */
    int
    minimum() const;

    /*!
     * Returns percentage.
     */
    int
    percentage() const;

    /*!
     * Returns the difference between maximum and minimum limits.
     */
    int
    range() const;

    /*!
     * Returns progress bar's value.
     */
    int
    value() const;

    /*!
     * Reset progress bar value and emit sigValueChanged.
     */
    void
    reset();

    /*!
     * Sets progress bar's direction.
     * @param _direction
     */
    void
    setOrientation(Orientation orientation);

    /*!
     * Sets progress bar's maximum limit.
     * @param maximum
     */
    void
    setMaximum(int maximum);

    /*!
     * Sets progress bar's minimum limit.
     * @param minimum
     */
    void
    setMinimum(int minimum);

    /*!
     * Sets progress bar's minimum and maximum limits.
     * @param minimum
     * @param maximum
     */
    void
    setRange(int minimum, int maximum);

    /*!
     * Sets progress bar's value. Emits sigValueChanged.
     * @param value
     */
    void
    setValue(int value);

    /*!
     * This signal is emitted when progress bar's value is changed.
     */
    sigc::signal<void, int> sigValueChanged;

private:
    //! This property holds the maximum value limit.
    int _maximum;
    //! This property holds the minimum value limit.
    int _minimum;
    //! This property holds the value shown on progress bar.
    int _value;
    //! This property holds the direction of the progress bar.
    Orientation _orientation;
    //! This is the difference between _maximum and _minimum. This is stored in order to simplify calculations.
    int _range;

    /*!
     * Paints ProgressBar on its surface using current designer.
     * @param rect Clipped rectangle to paint.
     */
    void
    compose(const PaintEvent& event);

};
}

#endif /* ILIXI_PROGRESSBAR_H_ */
