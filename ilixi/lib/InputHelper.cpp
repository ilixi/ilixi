/*
 Copyright 2010-2013 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>,
            Andreas Shimokawa <andi@directfb.org>.

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
#include <core/Logger.h>
#include <directfb.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_INPUTHELPER, "ilixi/lib/InputHelper", "InputHelper");

InputHelper::InputHelper()
        : _currentSegment(0),
          _currentCandidate(0)
{
    ILOG_TRACE_F(ILX_INPUTHELPER);
}

InputHelper::~InputHelper()
{
    ILOG_TRACE_F(ILX_INPUTHELPER);
}

void
InputHelper::setInputData(std::string data)
{
    ILOG_TRACE_F(ILX_INPUTHELPER);
    _data = data;
    preProcessInputData();
}

void
InputHelper::append(uint32_t symbol)
{
    ILOG_TRACE_F(ILX_INPUTHELPER);
    ILOG_DEBUG(ILX_INPUTHELPER, " -> symbol[%u] = %c\n", symbol, symbol);
    // ignore any symbols here...
    if (symbol == DIKS_CONTROL)
        return;
    else if (symbol == DIKS_BACKSPACE)
    {
        if (_data.length())
            _data.erase(_data.length() - 1, 1);
    } else
        _data += symbol;
    preProcessInputData();
}

const std::string&
InputHelper::getData() const
{
    return _data;
}

const std::string&
InputHelper::getPdata() const
{
    return _pdata;
}

int
InputHelper::getCurrentSegment() const
{
    return _currentSegment;
}

void
InputHelper::setCurrentSegment(int currentSegment)
{
    _currentSegment = currentSegment;
}

unsigned int
InputHelper::segments() const
{
    return _segments.size();
}

std::string
InputHelper::concatedSegments() const
{
    ILOG_TRACE_F(ILX_INPUTHELPER);
    std::string s;
    for (int i = 0; i < _segments.size(); ++i)
        s += _segments[i];
    return s;
}

std::string
InputHelper::getSegment(unsigned int index)
{
    if (index > _segments.size())
        return std::string("");
    return _segments[index];
}

void
InputHelper::getNextSegment()
{
    _currentCandidate = 0;
    _currentSegment++;
    if (_currentSegment >= _segments.size())
        _currentSegment = _segments.size() - 1;
    else
        sigUpdateUI();
}

void
InputHelper::getPreviousSegment()
{
    _currentCandidate = 0;
    _currentSegment--;
    if (_currentSegment < 0)
        _currentSegment = 0;
    else
        sigUpdateUI();
}

unsigned int
InputHelper::canditates() const
{
    return _candidates.size();
}

std::string
InputHelper::getCanditate(unsigned int index)
{
    if (index > _candidates.size())
        return std::string("");
    return _candidates[index];
}

void
InputHelper::getNextCandidate()
{
    _currentCandidate++;
    if (_currentCandidate >= _candidates.size())
        _currentCandidate = 0;
}

void
InputHelper::getPreviousCandidate()
{
    _currentCandidate--;
    if (_currentCandidate < 0)
        _currentCandidate = _candidates.size();
}

void
InputHelper::updateCurrentSegment()
{
    _segments[_currentSegment] = _candidates[_currentCandidate];
}

void
InputHelper::reset()
{
    _currentSegment = 0;
    _currentCandidate = 0;
    _data.clear();
    _pdata.clear();
    _segments.clear();
    _candidates.clear();
}

void
InputHelper::preProcessInputData()
{
}

} /* namespace ilixi */
