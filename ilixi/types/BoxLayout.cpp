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

#include <types/BoxLayout.h>

using namespace ilixi;

BoxLayout::BoxLayout(Widget* parent)
        : WidgetLayout(parent),
          _alignment(Alignment::Top)
{
}

BoxLayout::~BoxLayout()
{
}

Alignment::Vertical
BoxLayout::getVerticalAlignment() const
{
    return _alignment;
}

void
BoxLayout::setVerticalAlignment(Alignment::Vertical alignment)
{
    _alignment = alignment;
}

int
BoxLayout::heightForWidth(int width)
{
    return -1;
}

Size
BoxLayout::preferredSize()
{
    ElementList list;
    createCache(&list);

    int w = 0, h = 0;
    for (ElementList::const_iterator it = list.begin(); it != list.end(); ++it)
    {
        if (it->size.height() > h)
            h = it->size.height();

        w += it->size.width() + spacing();
    }

    return Size(w - spacing(), h);
}

void
BoxLayout::tile()
{
    if (!_modified)
        return;

    ElementList list;
    createCache(&list);

    if (list.size() == 0)
    {
        _modified = false;
        return;
    }

    ElementList w = list;
    int availableSpace = _bounds.width() - ((w.size() - 1) * spacing());
    int average = availableSpace / w.size();

    //***********************************************************
    // FixedConstraint
    //***********************************************************
    int usedSpace = 0;
    ElementList::iterator it = w.begin();
    while (it != w.end())
    {
        if (((CacheElement) *it).widget->xConstraint() == FixedConstraint
                && ((CacheElement) *it).widget->minWidth() < 0
                && ((CacheElement) *it).widget->maxWidth() < 0)
        {
            usedSpace += ((CacheElement) *it).size.width();
            it = w.erase(it);
        } else
            ++it;
    }

    if (usedSpace)
    {
        availableSpace -= usedSpace;
        if (w.size())
            average = availableSpace / w.size();
    }

    //***********************************************************
    // MinimumSize
    //***********************************************************
    usedSpace = 0;
    it = w.begin();
    while (it != w.end())
    {
        if (average < ((CacheElement) *it).widget->minWidth())
        {
            usedSpace += ((CacheElement) *it).widget->minWidth();
            it = w.erase(it);
        } else
            ++it;
    }

    if (usedSpace)
    {
        availableSpace -= usedSpace;
        if (w.size())
            average = availableSpace / w.size();
    }

    //***********************************************************
    // MaximumSize
    //***********************************************************
    usedSpace = 0;
    it = w.begin();
    while (it != w.end())
    {
        if (average > ((CacheElement) *it).widget->maxWidth()
                && ((CacheElement) *it).widget->maxWidth() > 0)
        {
            usedSpace += ((CacheElement) *it).widget->maxWidth();
            it = w.erase(it);
        } else
            ++it;
    }

    if (usedSpace)
    {
        availableSpace -= usedSpace;
        if (w.size())
            average = availableSpace / w.size();
    }

    //***********************************************************
    // ShrinkPolicy
    //***********************************************************
    usedSpace = 0;
    it = w.begin();
    while (it != w.end())
    {
        if (average < ((CacheElement) *it).size.width()
                && !(((CacheElement) *it).widget->xConstraint() & ShrinkPolicy))
        {
            usedSpace += ((CacheElement) *it).size.width();
            it = w.erase(it);
        } else
            ++it;
    }

    if (usedSpace)
    {
        availableSpace -= usedSpace;
        if (w.size())
            average = availableSpace / w.size();
    }

    //***********************************************************
    // GrowPolicy
    //***********************************************************
    usedSpace = 0;
    it = w.begin();
    while (it != w.end())
    {
        if (average > ((CacheElement) *it).size.width()
                && !(((CacheElement) *it).widget->xConstraint() & GrowPolicy))
        {
            usedSpace += ((CacheElement) *it).widget->width();
            it = w.erase(it);
        } else
            ++it;
    }

    if (usedSpace)
    {
        availableSpace -= usedSpace;
        if (w.size())
            average = availableSpace / w.size();
    }

    //***********************************************************
    // ExpandPolicy
    //***********************************************************
    int expanding = 0;
    int expandAverage = 0;
    int expandSpace = 0;
    it = w.begin();
    while (it != w.end())
    {
        if (((CacheElement) *it).widget->xConstraint() & ExpandPolicy)
            ++expanding;
        else
        {
            if (((CacheElement) *it).widget->minWidth() > 0
                    && average > ((CacheElement) *it).widget->minWidth())
            {
                expandSpace += average
                        - ((CacheElement) *it).widget->minWidth();
                ((CacheElement) *it).size.setWidth(
                        ((CacheElement) *it).widget->minWidth());
            } else if (average > ((CacheElement) *it).size.width())
                expandSpace += average - ((CacheElement) *it).size.width();
        }
        ++it;
    }

    if (expandSpace)
        expandAverage = expandSpace / expanding;

    //***********************************************************
    // Arrange widgets inside layout
    //***********************************************************
    int artifact = availableSpace - average * w.size();
    int currentX = 0;
    Widget* widget;
    Widget* left = _parent->getNeighbour(Left);
    Widget* right;
    Widget* up = _parent->getNeighbour(Up);
    Widget* down = _parent->getNeighbour(Down);
    ElementList::iterator itNext;
    for (ElementList::iterator it = list.begin(), end = list.end(); it != end;
            ++it)
    {
        widget = ((CacheElement) *it).widget;

        // Set width
        if (expanding)
        {
            if (widget->xConstraint() & ExpandPolicy)
            {
                if (artifact)
                {
                    widget->setWidth(average + expandAverage + artifact);
                    artifact = 0;
                } else
                    widget->setWidth(average + expandAverage);
            } else if (widget->xConstraint() & ShrinkPolicy
                    && average < ((CacheElement) *it).size.width())
                widget->setWidth(average);
            else
                widget->setWidth(((CacheElement) *it).size.width());
        } else
        {
            if (widget->minWidth() > 0 || widget->maxWidth() > 0)
                widget->setWidth(average);
            else if ((widget->xConstraint() & ShrinkPolicy)
                    && ((CacheElement) *it).size.width() > average)
                widget->setWidth(average);
            else if ((widget->xConstraint() & GrowPolicy)
                    && ((CacheElement) *it).size.width() < average)
            {
                if (artifact)
                {
                    widget->setWidth(average + artifact);
                    artifact = 0;
                } else
                    widget->setWidth(average);
            } else
                widget->setWidth(((CacheElement) *it).size.width());
        }

        widget->setHeight(((CacheElement) *it).size.height());

        // Set top-left using alignment.
        int y = 0;
        if (_alignment == Alignment::Bottom)
            y = (_bounds.height() - widget->height());
        else if (_alignment == Alignment::Middle)
            y = (_bounds.height() - widget->height()) / 2;
        widget->moveTo(currentX, y);

        currentX += widget->width() + spacing();

        // Set neighbours
        if (it == list.end())
            right = ((CacheElement) *list.begin()).widget;
        else
        {
            itNext = it;
            right = ((CacheElement) *(++itNext)).widget;
        }

        widget->setNeighbours(up, down, left, right);
        left = widget;
    }

    _modified = false;
}
