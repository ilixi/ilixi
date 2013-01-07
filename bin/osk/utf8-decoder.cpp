///////////////////////////////////////////////////////////////////
//
// Fast, small, simple, robust UTF-8 decoder in C
//
// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjo...@hoehrmann.de>
//
// This program is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
///////////////////////////////////////////////////////////////////

#include "utf8-decoder.h"

namespace ilixi
{
void
decode(uint8_t* s, std::vector<uint32_t>& ucs32)
{
    uint8_t data, byte, stat = 9;
    uint32_t unic = 0;

    // http://lists.w3.org/Archives/Public/www-archive/2009Apr/0000

    while ((byte = *s++))
    {

#if ASCII_IN_TABLE
        data = utf8d[byte];
        stat = utf8d[256 + (stat << 4) + (data >> 4)];
        byte = (byte ^ (uint8_t) (data << 4));
#else
        if (byte < 0x80)
        {
            stat = utf8d[128 + (stat << 4)];
        } else
        {
            data = utf8d[byte];
            stat = utf8d[128 + (stat << 4) + (data >> 4)];
            byte = (byte ^ (uint8_t) (data << 4));
        }
#endif

        unic = (unic << 6) | byte;

        if (!stat)
        {
            // unic is now a proper code point, we just print it out.
            ILOG_DEBUG(ILX_UTF8_DECODER, "U+%04X\n", unic);
            ucs32.push_back(unic);
            unic = 0;
        }

        if (stat == 1)
        {
            // the byte is not allowed here; the state would have to
            // be reset to continue meaningful reading of the string
        }

    }
}
}
