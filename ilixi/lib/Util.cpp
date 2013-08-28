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

#include <lib/Util.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sstream>
#include <iomanip>

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
    snprintf(uuid, 37, "%x%x-%x-%x-%x-%x%x%x", (uint16_t) rand(), (uint16_t) rand(), (uint16_t) rand(), (((uint16_t) rand() & 0x0fff) | 0x4000), (uint16_t) rand() % 0x3fff + 0x8000, (uint16_t) rand(), (uint16_t) rand(), (uint16_t) rand());
    return std::string(uuid);
}

std::string
PrintF(const char *format, ...)
{
    va_list args;
    size_t len;
    char buf[200];
    char *ptr = buf;

    va_start( args, format);
    len = vsnprintf(buf, sizeof(buf), format, args);
    va_end( args);

    if (len < 0)
        abort();

    if (len >= sizeof(buf))
    {
        ptr = (char*) malloc(len + 1);
        if (!ptr)
            abort();

        va_start( args, format);
        len = vsnprintf(ptr, len + 1, format, args);
        va_end( args);

        if (len < 0)
        {
            free(ptr);
            abort();
        }
    }

    std::string str(ptr);

    if (ptr != buf)
        free(ptr);

    return str;
}

std::string
toHMS(long int secs)
{
    int hour = secs / 3600;
    secs = (int) secs % 3600;
    if (hour)
        return PrintF("%.2d:%.2d:%.2d", secs / 3600, secs / 60, secs %60);
    else
        return PrintF("%.2d:%.2d", secs / 60, secs %60);
}

#define DIM(x) (sizeof(x)/sizeof(*(x)))

static const uint64_t eib_size = 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;
static const char* size_suffix[] = { "EB", "PB", "TB", "GB", "MB", "KB", "B" };

std::string
formatSize(uint64_t size)
{
    std::stringstream ss;
    ss.precision(2);
    uint64_t multiplier = eib_size;

    for (int i = 0; i < 7; i++, multiplier /= 1024)
    {
        if (size < multiplier)
            continue;
        ss << size / multiplier << size_suffix[i];
        return ss.str();
    }
    return ss.str();
}

std::string
formatDate(time_t rawtime)
{
    std::stringstream ss;
    struct tm * timeinfo;
    timeinfo = localtime(&rawtime);
    ss << timeinfo->tm_hour << ":" << timeinfo->tm_min << " " << timeinfo->tm_mday << "." << timeinfo->tm_mon << "." << timeinfo->tm_year + 1900;
    return ss.str();
}

} /* namespace ilixi */
