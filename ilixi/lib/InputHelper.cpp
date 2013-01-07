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

#include <lib/InputHelper.h>
#include <directfb.h>

namespace ilixi
{

InputHelper::InputHelper()
{
}

InputHelper::~InputHelper()
{
}

void
InputHelper::append(uint32_t symbol)
{
    if (symbol == DIKS_BACKSPACE && _conversion.size())
    {
//        printf("Converted len: %d\n", _conversion.length());
//        printf("BackSpace - step %d processed %d\n", _lastStep, _processed);
//        _conversion = _conversion.substr(0, _conversion.length() - _lastStep);
//        _buffer = _buffer.substr(0, _buffer.length() - _processed);
        sigConverted();
    } else if (symbol == DIKS_ENTER)
    {
        _buffer.clear();
        _conversion.clear();
        sigConverted();
    } else
    {
        _buffer += symbol;
        if (convert(symbol))
            sigConverted();
    }
}

std::wstring
InputHelper::conversion() const
{
    return _conversion;
}

unsigned int
InputHelper::canditates() const
{
    return _candidates.size();
}

std::wstring
InputHelper::getCanditate(unsigned int index)
{
    if (index > _candidates.size())
        index = _candidates.size();
    return _candidates[index];
}

} /* namespace ilixi */
