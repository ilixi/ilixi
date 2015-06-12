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

#include <types/LinearGradient.h>

namespace ilixi
{

LinearGradient::LinearGradient()
        : Gradient(Linear)
{
}

LinearGradient::LinearGradient(double x1, double y1, double x2, double y2)
        : Gradient(Linear)
{
    _pattern = cairo_pattern_create_linear(x1, y1, x2, y2);
}

LinearGradient::LinearGradient(const Point& start, const Point& end)
        : Gradient(Linear)
{
    _pattern = cairo_pattern_create_linear(start.x(), start.y(), end.x(), end.y());
}

LinearGradient::~LinearGradient()
{
}

void
LinearGradient::setPatternCoordinates(double x1, double y1, double x2, double y2)
{
    if (_pattern)
        cairo_pattern_destroy(_pattern);
    _pattern = cairo_pattern_create_linear(x1, y1, x2, y2);
}

void
LinearGradient::setPatternCoordinates(const Point& start, const Point& end)
{
    if (_pattern)
        cairo_pattern_destroy(_pattern);
    _pattern = cairo_pattern_create_linear(start.x(), start.y(), end.x(), end.y());
}

} /* namespace ilixi */
