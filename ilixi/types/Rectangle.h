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

#ifndef ILIXI_RECTANGLE_H_
#define ILIXI_RECTANGLE_H_

#include <lib/Util.h>
#include <types/Point.h>
#include <types/Size.h>

namespace ilixi
{
//! Defines a rectangle with top-left point and size using integer precision.
class Rectangle
{
public:
    /*!
     * Constructs a null rectangle.
     */
    Rectangle();

    /*!
     * Copy constructor.
     */
    Rectangle(const Rectangle& rectangle);

    /*!
     * Constructs a rectangle with the given topLeft corner and the given size.
     */
    Rectangle(const Point &topLeft, const Size &size);

    /*!
     * Constructs a rectangle with the given topLeft and bottomRight corners.
     */
    Rectangle(const Point &topLeft, const Point &bottomRight);

    /*!
     * Constructs a rectangle with (x, y) as its top-left corner and the given width and height.
     */
    Rectangle(int x, int y, int width, int height);

    /*!
     * Destructor.
     */
    ~Rectangle();

    /*!
     * Returns top-left x coordinate.
     */
    int
    x() const;

    /*!
     * Returns top-left y coordinate.
     */
    int
    y() const;

    /*!
     * Returns the width.
     */
    int
    width() const;

    /*!
     * Returns the height.
     */
    int
    height() const;

    /*!
     * Returns the size.
     */
    Size
    size() const;

    /*!
     * Returns the center point of rectangle.
     */
    Point
    center() const;

    /*!
     * Returns the bottom-left point of rectangle.
     */
    Point
    bottomLeft() const;

    /*!
     * Returns the bottom-right point of rectangle.
     */
    Point
    bottomRight() const;

    /*!
     * Returns the top-left point of rectangle.
     */
    Point
    topLeft() const;

    /*!
     * Returns the top-right point of rectangle.
     */
    Point
    topRight() const;

    /*!
     * Returns the bottom-right y coordinate.
     */
    int
    bottom() const;

    /*!
     * Returns the top-left x coordinate.
     */
    int
    left() const;

    /*!
     * Returns the bottom-right x corrdinate.
     */
    int
    right() const;

    /*!
     * Returns the top-left y coordinate.
     */
    int
    top() const;

    /*!
     * Returns true if the width and height are set to 0.
     */
    bool
    isNull() const;

    /*!
     * Returns true if rectangle is empty. ( left() > right() || top() > bottom())
     */
    bool
    isEmpty() const;

    /*!
     * Returns true if rectangle is valid. ((left() < right() && top() < bottom()))
     * @return
     */
    bool
    isValid() const;

    /*!
     * Returns true if given point is inside the rectangle.
     * @param point
     * @param edge if true considers points on edges as inside.
     */
    bool
    contains(const Point &point, bool edge = false) const;

    /*!
     * Returns true if point defined by (x, y) is inside the rectangle.
     * @param x in pixels.
     * @param y in pixels.
     * @param edge if true considers points on edges as inside.
     */
    bool
    contains(int x, int y, bool edge = false) const;

    /*!
     * Returns true if given rectangle is inside the rectangle.
     * @param rectangle
     * @param edge if true considers points on edges as inside.
     */
    bool
    contains(const Rectangle &rectangle, bool edge = false) const;

    /*!
     * Returns true if the rectangle intersects with the given rectangle.
     * @param rectangle
     */
    bool
    intersects(const Rectangle &rectangle) const;

    /*!
     * Returns the intersection of this rectangle with given rectangle.
     */
    Rectangle
    intersected(const Rectangle &rectangle) const;

    /*!
     * Returns the union of this rectangle and given rectangle.
     */
    Rectangle
    united(const Rectangle &rectangle) const;

    /*!
     * Converts this rectangle to the union of this rectangle and given rectangle.
     */
    void
    unite(const Rectangle &rectangle);

    /*!
     * Sets the top-left x and y coordinates.
     */
    void
    moveTo(int x, int y);

    /*!
     * Sets the top-left point.
     */
    void
    moveTo(const Point &point);

    /*!
     * Moves the rectangle's top-left position.
     */
    void
    translate(int x, int y);

    /*!
     * Sets the y coordinate of bottom-right corner.
     */
    void
    setBottom(int y);

    /*!
     * Sets the x coordinate of top-left corner.
     */
    void
    setLeft(int x);

    /*!
     * Sets the x coordinate of bottom-right corner.
     */
    void
    setRight(int x);

    /*!
     * Sets the y coordinate of top-left corner.
     */
    void
    setTop(int y);

    /*!
     * Sets the height of rectangle.
     */
    void
    setHeight(int height);

    /*!
     * Sets the width of rectangle.
     */
    void
    setWidth(int width);

    /*!
     * Sets the x coordinate of top-left corner.
     */
    void
    setX(int x);

    /*!
     * Sets the y coordinate of top-left corner.
     */
    void
    setY(int y);

    /*!
     * Sets the top-left position.
     */
    void
    setTopLeft(const Point& point);

    /*!
     * Sets the bottom-right position of rectangle.
     */
    void
    setBottomRight(const Point& point);

    /*!
     * Sets the width and height.
     */
    void
    setSize(int width, int height);

    /*!
     * Sets the size.
     */
    void
    setSize(const Size &size);

    /*!
     * Sets x, y coordinates and dimensions.
     */
    void
    setRectangle(int x, int y, int w, int h);

    /*!
     * Assignment operator.
     */
    Rectangle&
    operator=(const Rectangle &rectangle);

    /*!
     * Returns true if this rectangle is equal to r; otherwise returns false.
     */
    bool
    operator==(const Rectangle &r) const;

    /*!
     * Returns true if this rectangle is different from r; otherwise returns false.
     */
    bool
    operator!=(const Rectangle &r) const;

#if ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)
    /*!
     * Returns the DFBBox.
     */
    DFBBox
    dfbBox() const;
#endif
    /*!
     * Returns the DFBRectangle.
     */
    DFBRectangle
    dfbRect() const;

    /*!
     * Returns the DFBRegion.
     */
    DFBRegion
    dfbRegion() const;

private:
    //! This property stores the top-left point.
    Point _topLeft;
    //! This property stores the size.
    Size _size;

    friend std::istream&
    operator>>(std::istream& is, Rectangle& obj);

    friend std::ostream&
    operator<<(std::ostream& os, const Rectangle& obj);
};

inline int
Rectangle::x() const
{
    return _topLeft.x();
}

inline int
Rectangle::y() const
{
    return _topLeft.y();
}

inline int
Rectangle::width() const
{
    return _size.width();
}

inline int
Rectangle::height() const
{
    return _size.height();
}

inline int
Rectangle::left() const
{
    return _topLeft.x();
}

inline int
Rectangle::bottom() const
{
    return (_topLeft.y() + _size.height());
}

inline int
Rectangle::right() const
{
    return (_topLeft.x() + _size.width());
}

inline int
Rectangle::top() const
{
    return _topLeft.y();
}
}
#endif /* ILIXI_RECTANGLE_H_ */
