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

#include <compositor/Switcher.h>
#include <compositor/Compositor.h>
#include <core/Logger.h>
#include <algorithm>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_SWITCHER, "ilixi/compositor/Switcher", "Switcher");

Switcher::Switcher(ILXCompositor* compositor, Widget* parent)
        : Widget(parent),
          _compositor(compositor),
          _currentIndex(0)
{
    ILOG_TRACE_W(ILX_SWITCHER);
    setInputMethod(PointerPassthrough);
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
    if (_thumbs.size())
        scrollTo(nextThumb());
}

void
Switcher::scrollToPrevious()
{
    ILOG_TRACE_W(ILX_SWITCHER);
    if (_thumbs.size())
        scrollTo(previousThumb());
}

AppThumbnail*
Switcher::currentThumb()
{
    ILOG_TRACE_W(ILX_SWITCHER);
    return _thumbs[_currentIndex];
}

AppThumbnail*
Switcher::nextThumb()
{
    ILOG_TRACE_W(ILX_SWITCHER);
    if (_thumbs.size() == 0)
        return NULL;

    _currentIndex++;
    if (_currentIndex > _thumbs.size() - 1)
        _currentIndex = 0;
    return _thumbs[_currentIndex];
}

AppThumbnail*
Switcher::previousThumb()
{
    ILOG_TRACE_W(ILX_SWITCHER);
    if (_thumbs.size() == 0)
        return NULL;

    _currentIndex--;
    if (_currentIndex < 0)
        _currentIndex = _thumbs.size() - 1;
    return _thumbs[_currentIndex];
}

void
Switcher::setCurrentThumb(AppThumbnail* thumb)
{
    ILOG_TRACE_W(ILX_SWITCHER);
    for (int i = 0; i < _thumbs.size(); ++i)
    {
        if (thumb == _thumbs[i])
        {
            _currentIndex = i;
            break;
        }
    }
}

const Rectangle&
Switcher::getSwitcherGeometry() const
{
    return _switcherGeometry;
}

void
Switcher::setSwitcherGeometry(const Rectangle& rect)
{
    _switcherGeometry = rect;
    setGeometry(rect);
}

void
Switcher::killCurrentApp()
{
    _compositor->killApp(_thumbs[_currentIndex]->instance());
}

} /* namespace ilixi */

