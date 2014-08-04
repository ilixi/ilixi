/*
 Copyright 2010-2014 Tarik Sekmen.

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

#ifndef ILIXI_STYLEUTIL_H_
#define ILIXI_STYLEUTIL_H_

#include <types/Rectangle.h>
#include <libxml/tree.h>
#include <iostream>

namespace ilixi
{

//! This struct defines 3x1 (horizontal) or 1x3 (vertical) styled elements.
struct r3
{
    Rectangle l;
    Rectangle m;
    Rectangle r;
};

//! This struct defines 3x3 styled elements.
struct r9
{
    Rectangle tl;
    Rectangle tm;
    Rectangle tr;
    Rectangle l;
    Rectangle m;
    Rectangle r;
    Rectangle bl;
    Rectangle bm;
    Rectangle br;
};

void
getCell(xmlNodePtr node, Rectangle& dest);

void
getCellN(xmlNodePtr node, const Rectangle& ref, Rectangle& dest);

void
getCell3H(xmlNodePtr node, r3& dest);

void
getCell3HN(xmlNodePtr node, const r3& ref, r3& dest);

void
getCell3V(xmlNodePtr node, r3& dest);

void
getCell3VN(xmlNodePtr node, const r3& ref, r3& dest);

void
getCell9(xmlNodePtr node, r9& dest);

void
getCell9N(xmlNodePtr node, const r9& ref, r9& dest);

std::istream&
operator>>(std::istream& is, r3& obj);

std::ostream&
operator<<(std::ostream& os, const r3& obj);

std::istream&
operator>>(std::istream& is, r9& obj);

std::ostream&
operator<<(std::ostream& os, const r9& obj);

} /* namespace ilixi */
#endif /* ILIXI_STYLEUTIL_H_ */
