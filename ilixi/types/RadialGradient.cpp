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

#include <types/RadialGradient.h>

namespace ilixi
{

RadialGradient::RadialGradient(double x1, double y1, double radius1, double x2, double y2, double radius2)
        : Gradient(Radial)
{
    _pattern = cairo_pattern_create_radial(x1, y1, radius1, x2, y2, radius2);
}

RadialGradient::RadialGradient(const Point& center1, double radius1, const Point& center2, double radius2)
        : Gradient(Radial)
{
    _pattern = cairo_pattern_create_radial(center1.x(), center1.y(), radius1, center2.x(), center2.y(), radius2);
}

RadialGradient::~RadialGradient()
{
}

} /* namespace ilixi */
