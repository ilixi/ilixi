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

#include "Switcher.h"
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_SWITCHER, "ilixi/comp/Switcher", "Switcher");

Switcher::Switcher(Widget* parent)
        : Widget(parent),
          _current(NULL),
          _currentID(-1)
{
    ILOG_TRACE_W(ILX_SWITCHER);
    setInputMethod(PointerInput);
    setConstraints(ExpandingConstraint, FixedConstraint);
    setVisible(false);
}

Switcher::~Switcher()
{
    ILOG_TRACE_W(ILX_SWITCHER);
}

Size
Switcher::preferredSize() const
{
    ILOG_TRACE_W(ILX_SWITCHER);
    return Size(100, 100);
}

unsigned int
Switcher::itemCount() const
{
    return _thumbs.size();
}

void
Switcher::show()
{
    ILOG_TRACE_W(ILX_SWITCHER);
    setVisible(true);
}

void
Switcher::hide()
{
    ILOG_TRACE_W(ILX_SWITCHER);
    setVisible(false);
}

void
Switcher::scrollToNext()
{
    ILOG_TRACE_W(ILX_SWITCHER);
    if (_thumbs.size() > 1)
        scrollTo(nextThumb());
}

void
Switcher::scrollToPrevious()
{
    ILOG_TRACE_W(ILX_SWITCHER);
    if (_thumbs.size() > 1)
        scrollTo(previousThumb());
}

AppThumbnail*
Switcher::currentThumb()
{
    ILOG_TRACE_W(ILX_SWITCHER);
    return _current;
}

AppThumbnail*
Switcher::nextThumb()
{
    ILOG_TRACE_W(ILX_SWITCHER);
    if (_currentID >= 0)
    {
        if (++_currentID == _thumbs.size())
            _currentID = 0;

        return _thumbs[_currentID];
    }
    return NULL;
}

AppThumbnail*
Switcher::previousThumb()
{
    ILOG_TRACE_W(ILX_SWITCHER);
    if (_currentID >= 0)
    {
        if (--_currentID == -1)
            _currentID = _thumbs.size() - 1;

        return _thumbs[_currentID];
    }
    return NULL;
}

void
Switcher::setCurrentThumb(AppThumbnail* thumb)
{
    ILOG_TRACE_W(ILX_SWITCHER);
    _current = thumb;
    for (unsigned int i = 0; i < _thumbs.size(); ++i)
        if (_thumbs[i] == thumb)
            _currentID = i;
}

} /* namespace ilixi */
