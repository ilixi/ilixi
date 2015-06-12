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

#include <types/Gradient.h>

namespace ilixi
{

Gradient::Gradient()
        : _type(None),
          _pattern(0)
{
}

Gradient::Gradient(GradientType type)
        : _type(type),
          _pattern(0)
{
}

Gradient::Gradient(const Gradient& gradient)
        : _type(gradient.type()),
          _pattern(gradient.cairoGradient())
{
    if (_pattern)
        cairo_pattern_reference(_pattern);
}

Gradient::~Gradient()
{
    if (_pattern)
        cairo_pattern_destroy(_pattern);
}

int
Gradient::stops()
{
    int stops = 0;
    cairo_pattern_get_color_stop_count(_pattern, &stops);
    return stops;
}

cairo_pattern_t*
Gradient::cairoGradient() const
{
    return _pattern;
}

Gradient::GradientType
Gradient::type() const
{
    return _type;
}

void
Gradient::addStop(const Color& color, double offset)
{
    cairo_pattern_add_color_stop_rgba(_pattern, offset, color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0, color.alpha() / 255.0);
}

void
Gradient::addStop(double r, double g, double b, double a, double offset)
{
    cairo_pattern_add_color_stop_rgba(_pattern, offset, r / 255.0, g / 255.0, b / 255.0, a / 255.0);
}

void
Gradient::setExtendMethod(GradientExtendMethod extendType)
{
    cairo_pattern_set_extend(_pattern, (cairo_extend_t) extendType);
}

Gradient&
Gradient::operator=(const Gradient &gradient)
{
    if (this != &gradient)
    {
        _type = gradient.type();
        if (_pattern)
            cairo_pattern_destroy(_pattern);

        _pattern = gradient.cairoGradient();
        cairo_pattern_reference(_pattern);
    }
    return *this;
}

} /* namespace ilixi */
