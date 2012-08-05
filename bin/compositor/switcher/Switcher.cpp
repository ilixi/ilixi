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

namespace ilixi
{

  Switcher::Switcher(Widget* parent) :
      Widget(parent), _current(NULL), _currentID(-1)
  {
    setInputMethod(PointerInput);
    setConstraints(ExpandingConstraint, FixedConstraint);
    setVisible(false);
  }

  Switcher::~Switcher()
  {
  }

  Size
  Switcher::preferredSize() const
  {
    return Size(100, 100);
  }

  void
  Switcher::show()
  {
    setVisible(true);
  }

  void
  Switcher::hide()
  {
    setVisible(false);
  }

  void
  Switcher::scrollToNext()
  {
    scrollTo(nextThumb());
  }

  void
  Switcher::scrollToPrevious()
  {
    scrollTo(previousThumb());
  }

  AppThumbnail*
  Switcher::currentThumb()
  {
    return _current;
  }

  AppThumbnail*
  Switcher::nextThumb()
  {
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
    if (_currentID >= 0)
      {
        if (--_currentID == -1)
          _currentID = _thumbs.size() - 1;

        return _thumbs[_currentID];
      }
    return NULL;
  }

} /* namespace ilixi */
