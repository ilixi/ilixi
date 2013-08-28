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

#ifndef ILIXI_MARGIN_H_
#define ILIXI_MARGIN_H_

namespace ilixi
{
//! Defines margins.
class Margin
{
public:
    /*!
     * Creates a new 0 margin.
     */
    Margin();

    /*!
     * Creates a new margin using given value.
     * @param margin
     */
    Margin(int margin);

    /*!
     * Creates a new margin using given values.
     * @param top
     * @param bottom
     * @param left
     * @param right
     */
    Margin(int top, int bottom, int left, int right);

    /*!
     * Copy constructor.
     * @param margin
     */
    Margin(const Margin& margin);

    /*!
     * Destructor.
     */
    ~Margin();

    /*!
     * Returns bottom margin.
     */
    int
    bottom() const;

    /*!
     * Returns left margin.
     */
    int
    left() const;

    /*!
     * Returns right margin.
     */
    int
    right() const;

    /*!
     * Returns top margin.
     */
    int
    top() const;

    /*!
     * Returns right + left margin.
     */
    int
    hSum() const;

    /*!
     * Returns top + bottom margin.
     */
    int
    vSum() const;

    /*!
     * Sets bottom margin.
     * @param bottom
     */
    void
    setBottom(int bottom);

    /*!
     * Sets left margin.
     * @param left
     */
    void
    setLeft(int left);

    /*!
     * Sets right margin.
     * @param right
     */
    void
    setRight(int right);

    /*!
     * Sets top margin.
     * @param top
     */
    void
    setTop(int top);

    /*!
     * Sets margins.
     * @param top
     * @param bottom
     * @param left
     * @param right
     */
    void
    setMargins(int top, int bottom, int left, int right);

    /*!
     * Returns true if this margin is equal to m; otherwise returns false.
     */
    bool
    operator==(const Margin &m);

    /*!
     * Returns true if this margin is different from m; otherwise returns false.
     */
    bool
    operator!=(const Margin &m);

    /*!
     * Assigns m to this margin and returns a reference to it.
     */
    Margin&
    operator=(const Margin& m);

private:
    //! This property stores top margin.
    int _top;
    //! This property stores bottom margin.
    int _bottom;
    //! This property stores left margin.
    int _left;
    //! This property stores right margin.
    int _right;
};
}

#endif /* ILIXI_MARGIN_H_ */
