/*
 Copyright 2010-2015 Tarik Sekmen.

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

#ifndef ILIXI_RADIALGRADIENT_H_
#define ILIXI_RADIALGRADIENT_H_

#include <types/Gradient.h>
#include <types/Point.h>

namespace ilixi
{
//! Defines a radial gradient between a focal and an end point.
/*!
 * Radial gradients are used by a Brush to interpolate colors between a focal and an end point.
 *
 * Before using radial gradients, a number of color stops should be defined using addStop().
 */
class RadialGradient : public Gradient
{
public:

    /*!
     * Creates a new radial gradient.
     * @param x1 X coordinate of start circle.
     * @param y1 Y coordinate of start circle.
     * @param radius1 Radius of start circle.
     * @param x2 X coordinate of end circle.
     * @param y2 Y coordinate of end circle.
     * @param radius2 Radius of end circle.
     * @return
     */
    RadialGradient(double x1, double y1, double radius1, double x2, double y2, double radius2);

    /*!
     * Creates a new radial gradient.
     * @param center1 Center of start circle.
     * @param radius1 Radius of start circle.
     * @param center2 Center of end circle.
     * @param radius2 Radius of end circle.
     * @return
     */
    RadialGradient(const Point& center1, double radius1, const Point& center2, double radius2);

    /*!
     * Destructor.
     */
    ~RadialGradient();
};

} /* namespace ilixi */

#endif /* ILIXI_RADIALGRADIENT_H_ */
