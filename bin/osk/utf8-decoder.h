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

#ifndef UTF8_DECODER_H_
#define UTF8_DECODER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <core/Logger.h>
#include <vector>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_UTF8_DECODER, "ilixi/osk/utf8", "utf8");

#define ASCII_IN_TABLE 1

/*
 The first 128 entries are tuples of 4 bit values. The lower bits
 are a mask that when xor'd with a byte removes the leading utf-8
 bits. The upper bits are a character class number. The remaining
 160 entries are a minimal deterministic finite automaton. It has
 10 states and each state has 13 character class transitions, and
 3 unused transitions for padding reasons. When the automaton en-
 ters state zero, it has found a complete valid utf-8 code point;
 if it enters state one then the input sequence is not utf-8. The
 start state is state nine. Note the mixture of octal and decimal
 for stylistic reasons. The ASCII_IN_TABLE macro makes the array
 bigger and the code simpler--but not necessarily faster--if set.
 */

static const uint8_t utf8d[] = {

#if ASCII_IN_TABLE
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
#endif

        070, 070, 070, 070, 070, 070, 070, 070, 070, 070, 070, 070, 070, 070,
        070, 070, 050, 050, 050, 050, 050, 050, 050, 050, 050, 050, 050, 050,
        050, 050, 050, 050, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030,
        030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030, 030,
        030, 030, 030, 030, 030, 030, 030, 030, 204, 204, 188, 188, 188, 188,
        188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188,
        188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 174, 158,
        158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 142, 126, 126,
        111, 95, 95, 95, 79, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
        207,

        0, 1, 1, 1, 8, 7, 6, 4, 5, 4, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 1, 8, 7, 6, 4, 5, 4, 3, 2, 1, 1, 1, 1,

};

void
decode(uint8_t* s, std::vector<uint32_t>& ucs32);

} /* namespace ilixi */

#endif /* UTF8_DECODER_H_ */
