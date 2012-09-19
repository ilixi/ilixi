/*
 Copyright 2010-2012 Tarik Sekmen.

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

#include <lib/Util.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

namespace ilixi
{

unsigned int
createHash(const std::string& str)
{
    unsigned int hash = 5381;

    for (std::size_t i = 0; i < str.length(); i++)
        hash = ((hash << 5) + hash) + str[i];

    return hash;
}

std::string
createUUID()
{
    char uuid[37];
    srand(time(NULL));
    snprintf(uuid, 37, "%x%x-%x-%x-%x-%x%x%x", (uint16_t) rand(),
             (uint16_t) rand(), (uint16_t) rand(),
             (((uint16_t) rand() & 0x0fff) | 0x4000),
             (uint16_t) rand() % 0x3fff + 0x8000, (uint16_t) rand(),
             (uint16_t) rand(), (uint16_t) rand());
    return std::string(uuid);
}

} /* namespace ilixi */
