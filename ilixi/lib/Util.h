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

#ifndef ILIXI_UTIL_H_
#define ILIXI_UTIL_H_

#include <string>
#include <directfb_version.h>

extern "C"
{
#include <direct/util.h>
}

namespace ilixi
{

#if __GNUC__ > 3
#define DO_PRAGMA(x)    _Pragma (#x)
#define TODO(x)         DO_PRAGMA(message ("TODO - " #x))
#else
#define TODO(x)         do {} while(0)
#endif

#define VERSION_CODE(a,b,c)      (((a) << 16) + ((b) << 8) + (c))
#define ILIXI_DFB_VERSION       VERSION_CODE(DIRECTFB_MAJOR_VERSION, DIRECTFB_MINOR_VERSION, DIRECTFB_MICRO_VERSION)
#define ILIXI_HAS_SURFACEEVENTS ILIXI_DFB_VERSION >= VERSION_CODE(1,6,0)

/*!
 * Creates a unique hash value given a string.
 *
 * The algorithm was produced by Professor Daniel J. Bernstein.
 */
unsigned int
createHash(const std::string& str);

/*!
 * Creates a Version 4 UUID.
 *
 * @param uuid 36 characters long.
 */
std::string
createUUID();

std::string
PrintF(const char *format, ...) D_FORMAT_PRINTF( 1 );

std::string
toHMS(long int secs);

/*!
 * Converts size in bytes to string.
 */
std::string
formatSize(uint64_t size);

/*!
 * Converts time_t to a date string "HH:MM dd.mm.yyyy"
 */
std::string
formatDate(time_t rawtime);

} /* namespace ilixi */
#endif /* ILIXI_UTIL_H_ */
