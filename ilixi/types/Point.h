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

#ifndef ILIXI_POINT_H_
#define ILIXI_POINT_H_

#include <iostream>

namespace ilixi
{
//! Defines a point using integer precision.
/*!
 * Points are defined by an x and a y coordinate.
 */
class Point
{
public:
    /*!
     * Creates a new Point at (0, 0).
     */
    Point();

    /*!
     * Copy constructor.
     */
    Point(const Point& point);

    /*!
     * Creates a new point at (x, y).
     * @param x
     * @param y
     */
    Point(int x, int y);

    /*!
     * Returns x coordinate.
     */
    int
    x() const;

    /*!
     * Returns y coordinate.
     */
    int
    y() const;

    /*!
     * Returns true if both x and y are 0; otherwise false.
     */
    bool
    isNull() const;

    /*!
     * Sets x and y coordinates.
     * @param x
     * @param y
     */
    void
    moveTo(int x, int y);

    /*!
     * Sets x and y coordinates.
     * @param point
     */
    void
    moveTo(const Point &point);

    /*!
     * Adds x and y to this point's coordinates.
     * @param x
     * @param y
     */
    void
    translate(int x, int y);

    /*!
     * Sets x coordinate.
     * @param x
     */
    void
    setX(int x);

    /*!
     * Sets y coordinate.
     * @param y
     */
    void
    setY(int y);

    /*!
     * Assigns point to this font and returns a reference to it.
     * @param point
     */
    Point&
    operator=(const Point &point);

    /*!
     * Returns true if this point has the same x, yvalues as p; otherwise returns false.
     */
    bool
    operator==(const Point &p) const;

    /*!
     * Returns true if this point has a different x, y values from p; otherwise returns false.
     */
    bool
    operator!=(const Point &p) const;

private:
    //! This property stores the x coordinate.
    int _x;
    //! This property stores the y coordinate.
    int _y;

    friend std::istream&
    operator>>(std::istream& is, Point& obj);

    friend std::ostream&
    operator<<(std::ostream& os, const Point& obj);
};

inline int
Point::x() const
{
    return _x;
}

inline int
Point::y() const
{
    return _y;
}
}
#endif /* ILIXI_POINT_H_ */
