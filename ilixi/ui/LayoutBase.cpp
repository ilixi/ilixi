/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#include "ui/LayoutBase.h"
#include "ui/RadioButton.h"
#include "core/Logger.h"

using namespace ilixi;

LayoutBase::LayoutBase(Widget* parent) :
    Widget(parent), _modified(false), _spacing(5)
{
  ILOG_TRACE_W(ILX_LAYOUT);
  _group = new RadioGroup();
  setInputMethod(PointerInput);
}

LayoutBase::~LayoutBase()
{
  delete _group;
  ILOG_TRACE_W(ILX_LAYOUT);
}

int
LayoutBase::heightForWidth(int width) const
{
  return -1;
}

Size
LayoutBase::preferredSize() const
{
  ILOG_TRACE_W(ILX_LAYOUT);
  if (_children.size())
    {
      Rectangle r;
      Size s;
      for (WidgetList::const_iterator it = _children.begin();
          it != _children.end(); ++it)
        {
          Rectangle rTemp;
          rTemp.setTopLeft(((Widget*) *it)->position());
          if (((Widget*) *it)->size().isValid())
            rTemp.setSize(((Widget*) *it)->size());
          else
            rTemp.setSize(((Widget*) *it)->preferredSize());
          r.united(rTemp);
        }

      return r.size();
    }
  return Size(50, 50); // FIXME default size for layout.
}

unsigned int
LayoutBase::count() const
{
  return _children.size();
}

unsigned int
LayoutBase::spacing() const
{
  return _spacing;
}

void
LayoutBase::setSpacing(unsigned int spacing)
{
  if (_spacing != spacing)
    {
      _spacing = spacing;
      doLayout();
    }
}

bool
LayoutBase::addWidget(Widget* widget)
{
  if (addChild(widget))
    {
      doLayout();
      RadioButton* radio = dynamic_cast<RadioButton*>(widget);
      if (radio)
        _group->add(radio);
      return true;
    }
  return false;
}

bool
LayoutBase::removeWidget(Widget* widget)
{
  if (removeChild(widget))
    {
      // FIXME remove from radiogroup
//      RadioButton* radio = dynamic_cast<RadioButton*>(widget);
//      if (radio)
//        _group->remove(radio);

      doLayout();
      return true;
    }
  return false;
}

void
LayoutBase::tile()
{
  Widget* current;
  Widget* left = getNeighbour(Left);
  Widget* right = getNeighbour(Right);

  for (WidgetListConstIterator it = _children.begin(), itLast =
      (++_children.rbegin()).base(); it != _children.end(); ++it)
    {
      current = (Widget*) *it;
      if (current->visible())
        {

          if (!current->size().isValid())
            current->setSize(current->preferredSize());

          if (it != itLast)
            {
              WidgetListConstIterator itNext = it;
              right = (Widget*) *(++itNext);
            }
          else
            right = getNeighbour(Right);

          current->setNeighbours(left, right, left, right);
          left = current;
        }
    }
  _modified = false; // do not forget to set this flag to false!!
}

void
LayoutBase::doLayout()
{
  _modified = true;
  if (parent())
    parent()->doLayout();
}

void
LayoutBase::paint(const PaintEvent& event)
{
  if (visible())
    {
      PaintEvent evt(_frameGeometry, event);
      if (evt.isValid())
        {
          ILOG_TRACE_W(ILX_LAYOUT);
          updateSurface(evt);
          if (_modified)
            tile();
          paintChildren(evt);
        }
    }
}

void
LayoutBase::compose(const PaintEvent& event)
{
}

bool
LayoutBase::consumePointerEvent(const PointerEvent& pointerEvent)
{
  // priority is given to most recent child.
  for (WidgetListReverseIterator it = _children.rbegin();
      it != _children.rend(); ++it)
    {
      if (((Widget*) *it)->acceptsPointerInput()
          && ((Widget*) *it)->consumePointerEvent(pointerEvent))
        return true;
    }
  return false;
}

