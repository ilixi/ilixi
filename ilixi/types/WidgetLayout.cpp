/*
 Copyright 2011 Tarik Sekmen.

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

#include "types/WidgetLayout.h"
#include "core/Logger.h"
#include <algorithm>

using namespace ilixi;

WidgetLayout::WidgetLayout(Widget* parent) :
  _parent(parent), _modified(true), _spacing(5)
{
}

WidgetLayout::WidgetLayout(const WidgetLayout& layout)
{
}

WidgetLayout::~WidgetLayout()
{
}

int
WidgetLayout::heightForWidth(int width)
{
  return -1;
}

Size
WidgetLayout::preferredSize()
{
  ElementList list;
  createCache(&list);

  Rectangle r;
  for (ElementList::const_iterator it = list.begin(); it != list.end(); ++it)
    {
      Rectangle rTemp;
      rTemp.setTopLeft(it->widget->position());
      rTemp.setSize(it->size);
      r.united(rTemp);
    }

  return r.size();
}

Rectangle
WidgetLayout::bounds() const
{
  return _bounds;
}

unsigned int
WidgetLayout::spacing() const
{
  return _spacing;
}

void
WidgetLayout::setBounds(const Rectangle& rect)
{
  _bounds = rect;
  _modified = true;
}

void
WidgetLayout::setBounds(int x, int y, int w, int h)
{
  _bounds.setRectangle(x, y, w, h);
  _modified = true;
}

void
WidgetLayout::setSpacing(unsigned int spacing)
{
  _spacing = spacing;
  _modified = true;
}

void
WidgetLayout::setParent(Widget* parent)
{
  _parent = parent;
}

bool
WidgetLayout::addWidget(Widget* widget)
{
  if (!widget)
    return false;

  Widget::WidgetListConstIterator it = std::find(_widgets.begin(),
      _widgets.end(), widget);
  if (widget == *it)
    {
      ILOG_WARNING("Cannot add duplicate widget %p!", widget);
      return false;
    }

  _widgets.push_back(widget);
  return true;
}

bool
WidgetLayout::removeWidget(Widget* widget)
{
  if (!widget)
    return false;

  Widget::WidgetListIterator it = std::find(_widgets.begin(), _widgets.end(),
      widget);
  if (widget == *it)
    {
      _widgets.erase(it);
      return true;
    }
  return false;
}

void
WidgetLayout::invalidate()
{
  _modified = true;
}

void
WidgetLayout::tile()
{
  if (!_modified)
    return;

  // createCache if necessary
  // modify geometry according to constraints
  // assign neighbours
  _modified = false;
}

void
WidgetLayout::createCache(ElementList* cache)
{
  cache->clear();
  CacheElement e;
  for (Widget::WidgetListConstIterator it = _widgets.begin(); it
      != _widgets.end(); ++it)
    {
      e.widget = ((Widget*) *it);
      if (e.widget->visible() && (e.widget->xConstraint() != IgnoredConstraint
          || e.widget->yConstraint() != IgnoredConstraint))
        {
          e.size = e.widget->preferredSize();

          // handle min-max width (min has priority)
          if (e.size.width() < e.widget->minWidth())
            e.size.setWidth(e.widget->minWidth());
          else if (e.widget->maxWidth() > 0 && e.size.width()
              > e.widget->maxWidth())
            e.size.setWidth(e.widget->maxWidth());

          // handle min-max height (min has priority)
          if (e.size.height() < e.widget->minHeight())
            e.size.setHeight(e.widget->minHeight());
          else if (e.widget->maxHeight() > 0 && e.size.height()
              > e.widget->maxHeight())
            e.size.setHeight(e.widget->maxHeight());

          cache->push_back(e);
        }
    }
}
