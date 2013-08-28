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

#ifndef ILIXI_SIZE_H_
#define ILIXI_SIZE_H_

#include <directfb.h>

namespace ilixi
{
//! Defines width and height using integers.
/*!
 * Specifies a dimension using width and height.
 */
class Size
{
public:
    /*!
     * Constructs an invalid size, i.e. width = height = -1.
     */
    Size();

    /*!
     * Copy constructor.
     */
    Size(const Size& size);

    /*!
     * Constructs a size with given width and height.
     *
     * @param width in pixels.
     * @param height in pixels.
     */
    Size(int width, int height);

    /*!
     * Destructor.
     */
    ~Size();

    /*!
     * Returns true if both the width and height is equal to or greater than 0; otherwise returns false.
     */
    bool
    isValid() const;

    /*!
     * Returns the height.
     */
    int
    height() const;

    /*!
     * Returns the width.
     */
    int
    width() const;

    /*!
     * Swaps the width and height values.
     */
    void
    transpose();

    /*!
     * Sets the height.
     *
     * @param height in pixels.
     */
    void
    setHeight(int height);

    /*!
     * Sets the width.
     *
     * @param width in pixels.
     */
    void
    setWidth(int width);

    /*!
     * Assignment operator.
     */
    Size&
    operator=(const Size &size);

    /*!
     * Returns true if this size is equal to m; otherwise returns false.
     */
    bool
    operator==(const Size &s) const;

    /*!
     * Returns true if this size is different from m; otherwise returns false.
     */
    bool
    operator!=(const Size &s) const;

private:
    //! This property stores the width in pixels.
    int _width;
    //! This property stores the height in pixels.
    int _height;
};

inline int
Size::height() const
{
    return _height;
}

inline int
Size::width() const
{
    return _width;
}
}
#endif /* ILIXI_SIZE_H_ */
