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

#include "ui/HBoxLayout.h"
#include "core/Logger.h"

using namespace ilixi;

HBoxLayout::HBoxLayout(Widget* parent) :
    LayoutBase(parent), _alignment(Alignment::Top)
{
  //  setConstraints(MinimumConstraint, MinimumConstraint);
}

HBoxLayout::~HBoxLayout()
{
}

Alignment::Vertical
HBoxLayout::getVerticalAlignment() const
{
  return _alignment;
}

void
HBoxLayout::setVerticalAlignment(Alignment::Vertical alignment)
{
  _alignment = alignment;
}

int
HBoxLayout::heightForWidth(int width) const
{
  // TODO This method is not working...
  int h = 0; // max. height
  int hTemp = 0; // height for current widget
  int h4w = 0;
  Widget* widget;
  for (WidgetList::const_iterator it = _children.begin(); it != _children.end();
      ++it)
    {
      widget = ((Widget*) *it);
      if (widget->visible() && widget->xConstraint() != IgnoredConstraint)
        {
          hTemp = widget->preferredSize().height();

          // update hTemp if widget permits resizing in vertical axis.
          h4w = widget->heightForWidth(width);
          if (h4w)
            {
              if (h4w > hTemp && widget->yConstraint() & GrowPolicy)
                hTemp = h4w;

              else if (h4w < hTemp && widget->yConstraint() & ShrinkPolicy)
                hTemp = h4w;
            }

          // handle min-max height (min has priority)
          if (hTemp < widget->minHeight())
            hTemp = widget->minHeight();
          else if (widget->maxHeight() > 0 && hTemp > widget->maxHeight())
            hTemp = widget->maxHeight();

          // find max. height
          if (hTemp > h)
            h = hTemp;
        }
    }
  return h + spacing();
}

Size
HBoxLayout::preferredSize() const
{
  Widget* widget; // current widget
  int w = 0; // total width
  int h = 0; // max. height
  int cw = 0; // current widget's width.
  int ch = 0; // current widget's height.
  Size s;

  for (WidgetList::const_iterator it = _children.begin(); it != _children.end();
      ++it)
    {
      widget = ((Widget*) *it);
      if (widget->visible() && widget->xConstraint() != IgnoredConstraint)
        {
          s = widget->preferredSize();
          cw = s.width();
          ch = s.height();

          // satisfy min-max width
          if (widget->minWidth() > 0 && cw < widget->minWidth())
            cw = widget->minWidth();
          else if (widget->maxWidth() > 0 && cw > widget->maxWidth())
            cw = widget->maxWidth();

          // satisfy min-max height
          if (widget->minHeight() > 0 && ch < widget->minHeight())
            ch = widget->minHeight();
          else if (widget->maxHeight() > 0 && ch > widget->maxHeight())
            ch = widget->maxHeight();

          if (ch > h)
            h = ch;

          w += cw + spacing();
        }
    }

  return Size(w - spacing(), h);
}

void
HBoxLayout::tile()
{
  ElementList list;
  LayoutElement e;
  for (Widget::WidgetListConstIterator it = _children.begin();
      it != _children.end(); ++it)
    {
      e.widget = ((Widget*) *it);
      if (e.widget->visible() && e.widget->xConstraint() != IgnoredConstraint)
        {
          e.size = e.widget->preferredSize();
          list.push_back(e);
        }
    }

  if (list.size() == 0)
    {
      _modified = false;
      return;
    }

  ElementList l = list;
  int availableSpace = width() - ((l.size() - 1) * spacing());
  int average = availableSpace / l.size();

  //***********************************************************
  // FixedConstraint
  //***********************************************************
  int usedSpace = 0;
  ElementList::iterator it = l.begin();
  while (it != l.end())
    {
      if (((LayoutElement) *it).widget->xConstraint() == FixedConstraint
          && ((LayoutElement) *it).widget->minWidth() < 0
          && ((LayoutElement) *it).widget->maxWidth() < 0)
        {
          usedSpace += ((LayoutElement) *it).size.width();
          it = l.erase(it);
        }
      else
        ++it;
    }

  if (usedSpace)
    {
      availableSpace -= usedSpace;
      if (l.size())
        average = availableSpace / l.size();
    }

  //***********************************************************
  // MinimumSize
  //***********************************************************
  usedSpace = 0;
  it = l.begin();
  while (it != l.end())
    {
      if (average < ((LayoutElement) *it).widget->minWidth())
        {
          usedSpace += ((LayoutElement) *it).widget->minWidth();
          it = l.erase(it);
        }
      else
        ++it;
    }

  if (usedSpace)
    {
      availableSpace -= usedSpace;
      if (l.size())
        average = availableSpace / l.size();
    }

  //***********************************************************
  // MaximumSize
  //***********************************************************
  usedSpace = 0;
  it = l.begin();
  while (it != l.end())
    {
      if (average > ((LayoutElement) *it).widget->maxWidth()
          && ((LayoutElement) *it).widget->maxWidth() > 0)
        {
          usedSpace += ((LayoutElement) *it).widget->maxWidth();
          it = l.erase(it);
        }
      else
        ++it;
    }

  if (usedSpace)
    {
      availableSpace -= usedSpace;
      if (l.size())
        average = availableSpace / l.size();
    }

  //***********************************************************
  // ShrinkPolicy
  //***********************************************************
  usedSpace = 0;
  it = l.begin();
  while (it != l.end())
    {
      if (average < ((LayoutElement) *it).size.width()
          && !(((LayoutElement) *it).widget->xConstraint() & ShrinkPolicy))
        {
          usedSpace += ((LayoutElement) *it).size.width();
          it = l.erase(it);
        }
      else
        ++it;
    }

  if (usedSpace)
    {
      availableSpace -= usedSpace;
      if (l.size())
        average = availableSpace / l.size();
    }

  //***********************************************************
  // GrowPolicy
  //***********************************************************
  usedSpace = 0;
  it = l.begin();
  while (it != l.end())
    {
      if (average > ((LayoutElement) *it).size.width()
          && !(((LayoutElement) *it).widget->xConstraint() & GrowPolicy))
        {
          usedSpace += ((LayoutElement) *it).widget->width();
          it = l.erase(it);
        }
      else
        ++it;
    }

  if (usedSpace)
    {
      availableSpace -= usedSpace;
      if (l.size())
        average = availableSpace / l.size();
    }

  //***********************************************************
  // ExpandPolicy
  //***********************************************************
  int expanding = 0;
  int expandAverage = 0;
  int expandSpace = 0;
  it = l.begin();
  while (it != l.end())
    {
      if (((LayoutElement) *it).widget->xConstraint() & ExpandPolicy)
        ++expanding;
      else
        {
          if (((LayoutElement) *it).widget->minWidth() > 0
              && average > ((LayoutElement) *it).widget->minWidth())
            {
              expandSpace += average - ((LayoutElement) *it).widget->minWidth();
              ((LayoutElement) *it).size.setWidth(
                  ((LayoutElement) *it).widget->minWidth());
            }
          else if (average > ((LayoutElement) *it).size.width())
            expandSpace += average - ((LayoutElement) *it).size.width();
        }
      ++it;
    }

  if (expandSpace && expanding)
    expandAverage = expandSpace / expanding;

  //***********************************************************
  // Arrange widgets inside layout
  //***********************************************************
  int artifact = availableSpace - average * l.size();
  int currentX = 0;
  Widget* widget;
  Widget* left = getNeighbour(Left);
  Widget* right = getNeighbour(Right);
  Widget* up = getNeighbour(Up);
  Widget* down = getNeighbour(Down);
  ElementList::iterator itNext;
  for (ElementList::iterator it = list.begin(), end = list.end(); it != end;
      ++it)
    {
      widget = ((LayoutElement) *it).widget;

      // Set width
      if (expanding)
        {
          if (widget->xConstraint() & ExpandPolicy)
            {
              if (artifact)
                {
                  widget->setWidth(average + expandAverage + artifact);
                  artifact = 0;
                }
              else
                widget->setWidth(average + expandAverage);
            }
          else if (widget->xConstraint() & ShrinkPolicy
              && average < ((LayoutElement) *it).size.width())
            widget->setWidth(average);
          else
            widget->setWidth(((LayoutElement) *it).size.width());
        }
      else
        {
          if (widget->minWidth() > 0 || widget->maxWidth() > 0)
            widget->setWidth(average);
          else if ((widget->xConstraint() & ShrinkPolicy)
              && ((LayoutElement) *it).size.width() > average)
            widget->setWidth(average);
          else if ((widget->xConstraint() & GrowPolicy)
              && ((LayoutElement) *it).size.width() < average)
            {
              if (artifact)
                {
                  widget->setWidth(average + artifact);
                  artifact = 0;
                }
              else
                widget->setWidth(average);
            }
          else
            widget->setWidth(((LayoutElement) *it).size.width());
        }

      // Set height
      if (widget->yConstraint() != FixedConstraint)
        {
          // check if widget provides a height for width, if not use
          // height for hbox instead.
          int h4w = widget->heightForWidth(widget->width());
          if (h4w > 0)
            {
              // check grow-shrink for height
              if (((LayoutElement) *it).size.height() < h4w
                  && widget->yConstraint() & GrowPolicy)
                widget->setHeight(h4w);
              else if (((LayoutElement) *it).size.height() >= h4w
                  && widget->yConstraint() & ShrinkPolicy)
                widget->setHeight(h4w);
            }
          else
            {
              if (((LayoutElement) *it).size.height() < height()
                  && widget->yConstraint() & GrowPolicy)
                widget->setHeight(height());
              else if (((LayoutElement) *it).size.height() >= height()
                  && widget->yConstraint() & ShrinkPolicy)
                widget->setHeight(height());
            }
        }
      else
        widget->setHeight(((LayoutElement) *it).size.height());

      // Set top-left using alignment.
      int y = 0;
      if (_alignment == Alignment::Bottom)
        y = (height() - widget->height());
      else if (_alignment == Alignment::Middle)
        y = (height() - widget->height()) / 2;
      widget->moveTo(currentX, y);

      currentX += widget->width() + spacing();

      // Set neighbours
      if (it == list.end())
        right = getNeighbour(Right);
      else
        {
          itNext = it;
          right = ((LayoutElement) *(++itNext)).widget;
        }

      widget->setNeighbours(up, down, left, right);
      left = widget;
    }

  _modified = false;
}
