/*
 Copyright 2010-2012 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>,
            Sonat Deniz <sonat@ilixi.org>.

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

#include "Util.h"

double
production(std::string op, double l, double r)
{
    if (op == "+")
    {
        return l + r;
    } else if (op == "-")
    {
        return l - r;
    } else if (op == "/")
    {
        if (r == 0)
        {
            throw Exception("Divide by zero!");
        }
        return l / r;
    } else if (op == "*")
    {
        return l * r;
    } else if (op == "^")
    {
        return std::pow(l, r);
    } else if (op == "%")
    {
        return fmod(l, r);
    }
    return 0;
}

double
singleproduction(std::string op, double elem)
{
    if (op == "-")
    {
        return (-1 * elem);
    } else if (op == "|")
    {
        return sqrt(elem);
    }

    return 0;
}

double
toD(std::string b)
{
    std::istringstream i(b);
    double x;
    if (!(i >> x))
    {
        return 0;
    }
    return x;
}
