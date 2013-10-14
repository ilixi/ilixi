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

#ifndef ILIXI_LINEARGRADIENT_H_
#define ILIXI_LINEARGRADIENT_H_

#include <types/Point.h>
#include <types/Gradient.h>

namespace ilixi
{
//! Defines a linear gradient between two end points.
/*!
 * Linear gradients are used by a Brush to interpolate colors between a start and an end point.
 *
 * Before using radial gradients, a number of color stops should be defined using addStop().
 */
class LinearGradient : public Gradient
{
public:
    /*!
     * Creates an invalid pattern.
     */
    LinearGradient();

    /*!
     * Creates a new linear gradient.
     *
     * @param x1 X coordinate of start point.
     * @param y1 Y coordinate of start point.
     * @param x2 X coordinate of end point.
     * @param y2 Y coordinate of end point.
     */
    LinearGradient(double x1, double y1, double x2, double y2);

    /*!
     * Creates a new linear gradient.
     *
     * @param start Start point.
     * @param end End point.
     */
    LinearGradient(const Point& start, const Point& end);

    /*!
     * Destructor.
     */
    ~LinearGradient();

    /*!
     * Creates a new linear gradient and deletes old one.
     *
     * @param x1 X coordinate of start point.
     * @param y1 Y coordinate of start point.
     * @param x2 X coordinate of end point.
     * @param y2 Y coordinate of end point.
     */
    void
    setPatternCoordinates(double x1, double y1, double x2, double y2);

    /*!
     * Creates a new linear gradient and deletes old one.
     *
     * @param start Start point.
     * @param end End point.
     */
    void
    setPatternCoordinates(const Point& start, const Point& end);
};

} /* namespace ilixi */

#endif /* ILIXI_LINEARGRADIENT_H_ */
