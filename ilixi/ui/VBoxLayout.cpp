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

#include <ui/VBoxLayout.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_VBOX, "ilixi/ui/VBox", "VBox");

VBoxLayout::VBoxLayout(Widget* parent)
        : LayoutBase(parent),
          _alignment(Alignment::Left)
{
    ILOG_TRACE_W(ILX_VBOX);
    setConstraints(MinimumConstraint, MinimumConstraint);
}

VBoxLayout::~VBoxLayout()
{
    ILOG_TRACE_W(ILX_VBOX);
}

Alignment::Horizontal
VBoxLayout::geHorizontalAlignment() const
{
    return _alignment;
}

void
VBoxLayout::setHorizontalAlignment(Alignment::Horizontal alignment)
{
    _alignment = alignment;
}

bool
VBoxLayout::insertWidget(unsigned int row, Widget* widget)
{
    if (insertChild(widget, row))
    {
        doLayout();
        return true;
    }
    return false;
}

int
VBoxLayout::heightForWidth(int width) const
{
    if (!_children.size())
        return -1;
    ILOG_TRACE_W(ILX_VBOX);
    Widget* widget; // current widget
    int h = 0; // total height
    int cw = 0; // current widget's width.
    int ch = 0; // current widget's height.
    Size s;

    for (WidgetList::const_iterator it = _children.begin(); it != _children.end(); ++it)
    {
        widget = ((Widget*) *it);
        if (widget->visible() && widget->yConstraint() != IgnoredConstraint)
        {
            s = widget->preferredSize();
            cw = s.width();
            ch = s.height();

            // check grow-shrink
            if (cw < width && (widget->xConstraint() & GrowPolicy))
                cw = width;
            else if (cw > width && (widget->xConstraint() & ShrinkPolicy))
                cw = width;

            // satisfy min-max width
            if (widget->minWidth() > 0 && cw < widget->minWidth())
                cw = widget->minWidth();
            else if (widget->maxWidth() > 0 && cw > widget->maxWidth())
                cw = widget->maxWidth();

            if (cw != s.width())
            {
                // calculate h4w
                int h4w = widget->heightForWidth(cw);
                if (h4w)
                {
                    // check grow-shrink
                    if (ch < h4w && (widget->yConstraint() & GrowPolicy))
                        ch = h4w;
                    else if (ch > h4w && (widget->yConstraint() & ShrinkPolicy))
                        ch = h4w;
                }
            }

            // satisfy min-max height
            if (widget->minHeight() > 0 && ch < widget->minHeight())
                ch = widget->minHeight();
            else if (widget->maxHeight() > 0 && ch > widget->maxHeight())
                ch = widget->maxHeight();

            h += ch + spacing();
        }
    }

    if (h)
        return h - spacing();
    return -1;
}

Size
VBoxLayout::preferredSize() const
{
    ILOG_TRACE_W(ILX_VBOX);

    if (!_children.size())
        return Size(50, 50);

    ElementList l;      // temporary widget list
    LayoutElement e;    // Current element
    int w = 0;          // max. width.
    int h = 0;          // total height.
    int cw = 0;         // current widget's width.
    int ch = 0;         // current widget's height.

    // Find max. width
    for (WidgetList::const_iterator it = _children.begin(); it != _children.end(); ++it)
    {
        e.widget = ((Widget*) *it);
        if (e.widget->visible() && e.widget->yConstraint() != IgnoredConstraint)
        {
            e.size = e.widget->preferredSize();

            // satisfy min-max width
            if (e.widget->minWidth() > 0 && e.size.width() < e.widget->minWidth())
                e.size.setWidth(e.widget->minWidth());
            else if (e.widget->maxWidth() > 0 && e.size.width() > e.widget->maxWidth())
                e.size.setWidth(e.widget->maxWidth());

            if (e.size.width() > w)
                w = e.size.width();

            l.push_back(e);
        }
    }

    // Calculate total height
    Widget* widget; // current widget
    for (ElementList::const_iterator it = l.begin(); it != l.end(); ++it)
    {
        widget = ((LayoutElement) *it).widget;
        cw = it->size.width();
        ch = it->size.height();

        // check if widget can grow/shrink on x axis
        if (cw < w && (widget->xConstraint() & GrowPolicy))
            cw = w;
        else if (cw >= w && (widget->xConstraint() & ShrinkPolicy))
            cw = w;

        if (cw != it->size.width())
        {
            int h4w = widget->heightForWidth(cw);
            if (h4w)
            {
                // check if widget can grow/shrink on y axis
                if (ch < h4w && (widget->yConstraint() & GrowPolicy))
                    ch = h4w;
                else if (ch > h4w && (widget->yConstraint() & ShrinkPolicy))
                    ch = h4w;
            }
        }

        // satisfy min-max height
        if (widget->minHeight() > 0 && ch < widget->minHeight())
            ch = widget->minHeight();
        else if (widget->maxHeight() > 0 && ch > widget->maxHeight())
            ch = widget->maxHeight();

        h += ch + spacing();
    }
    ILOG_DEBUG(ILX_VBOX, " -> Size(%d, %d)\n", w, h - spacing());
    return Size(w, h - spacing());
}

void
VBoxLayout::tile()
{
    ILOG_TRACE_W(ILX_VBOX);
    ElementList list;
    LayoutElement e;
    int cw = 0; // current widget's width.
    for (Widget::WidgetListConstIterator it = _children.begin(); it != _children.end(); ++it)
    {
        e.widget = ((Widget*) *it);
        if (e.widget->visible() && e.widget->yConstraint() != IgnoredConstraint)
        {
            e.size = e.widget->preferredSize();
            cw = e.size.width();

            // check grow-shrink for width
            if (cw > width() && (e.widget->xConstraint() & ShrinkPolicy))
                cw = width();
            else if (cw < width() && (e.widget->xConstraint() & GrowPolicy))
                cw = width();

            // satisfy min-max width
            if (e.widget->minWidth() > 0 && cw < e.widget->minWidth())
                cw = e.widget->minWidth();
            else if (e.widget->maxWidth() > 0 && cw > e.widget->maxWidth())
                cw = e.widget->maxWidth();

            if (cw != e.size.width())
            {
                e.size.setWidth(cw);
                //              int h4w = e.widget->heightForWidth(cw);
                //              if (h4w)
                //                {
                //                  // check grow-shrink for height
                //                  if (h4w > e.size.height() && e.widget->yConstraint()
                //                      & GrowPolicy)
                //                    e.size.setHeight(h4w);
                //                  else if (h4w < e.size.height() && e.widget->yConstraint()
                //                      & ShrinkPolicy)
                //                    e.size.setHeight(h4w);
                //                }
            }

            list.push_back(e);
        }
    }

    if (list.size() == 0)
    {
        _modified = false;
        return;
    }

    ElementList l = list;
    int availableSpace = height() - ((l.size() - 1) * spacing());
    int average = availableSpace / l.size();
    //***********************************************************
    // FixedConstraint
    //***********************************************************
    int usedSpace = 0;
    ElementList::iterator it = l.begin();
    while (it != l.end())
    {
        if (((LayoutElement) *it).widget->yConstraint() == FixedConstraint && ((LayoutElement) *it).widget->minHeight() < 0 && ((LayoutElement) *it).widget->maxHeight() < 0)
        {
            usedSpace += ((LayoutElement) *it).size.height();
            it = l.erase(it);
        } else
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
        if (average > ((LayoutElement) *it).widget->maxHeight() && ((LayoutElement) *it).widget->maxHeight() > 0)
        {
            usedSpace += ((LayoutElement) *it).widget->maxHeight();
            it = l.erase(it);
        } else
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
        if (average < ((LayoutElement) *it).widget->minHeight())
        {
            usedSpace += ((LayoutElement) *it).widget->minHeight();
            it = l.erase(it);
        } else
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
        if (average < ((LayoutElement) *it).size.height() && !(((LayoutElement) *it).widget->yConstraint() & ShrinkPolicy))
        {
            usedSpace += ((LayoutElement) *it).size.height();
            it = l.erase(it);
        } else
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
        if (average > ((LayoutElement) *it).size.height() && !(((LayoutElement) *it).widget->yConstraint() & GrowPolicy))
        {
            usedSpace += ((LayoutElement) *it).widget->height();
            it = l.erase(it);
        } else
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
        if (((LayoutElement) *it).widget->yConstraint() & ExpandPolicy)
            ++expanding;
        else
        {
            if (((LayoutElement) *it).widget->minHeight() > 0 && average > ((LayoutElement) *it).widget->minHeight())
            {
                expandSpace += average - ((LayoutElement) *it).widget->minHeight();
                ((LayoutElement) *it).size.setHeight(((LayoutElement) *it).widget->minHeight());
            } else if (average > ((LayoutElement) *it).size.height())
                expandSpace += average - ((LayoutElement) *it).size.height();
        }
        ++it;
    }
    if (expandSpace && expanding)
        expandAverage = expandSpace / expanding;

    //***********************************************************
    // Arrange widgets inside layout
    //***********************************************************
    int artifact = availableSpace - average * l.size();
    int currentY = 0;
    Widget* widget;
    Widget* left = getNeighbour(Left);
    Widget* right = getNeighbour(Right);
    Widget* up = _keyNavChildrenFirst ? list.back().widget : getNeighbour(Up);
    Widget* down = getNeighbour(Down);
    ElementList::iterator itNext, itLast;
    for (ElementList::iterator it = list.begin(), end = list.end(), itLast = (++list.rbegin()).base(); it != end; ++it)
    {
        widget = ((LayoutElement) *it).widget;

        // Set height
        if (expanding)
        {
            if (widget->yConstraint() & ExpandPolicy)
            {
                if (artifact)
                {
                    widget->setHeight(average + expandAverage + artifact);
                    artifact = 0;
                } else
                    widget->setHeight(average + expandAverage);
            } else if ((widget->yConstraint() & ShrinkPolicy) && average < ((LayoutElement) *it).size.height())
                widget->setHeight(average);
            else
                widget->setHeight(((LayoutElement) *it).size.height());
        } else
        {
            if (widget->minHeight() > 0 || widget->maxHeight() > 0)
                widget->setHeight(average);
            else if ((widget->yConstraint() & ShrinkPolicy) && ((LayoutElement) *it).size.height() > average)
                widget->setHeight(average);
            else if ((widget->yConstraint() & GrowPolicy) && ((LayoutElement) *it).size.height() < average)
            {
                if (artifact)
                {
                    widget->setHeight(average + artifact);
                    artifact = 0;
                } else
                    widget->setHeight(average);
            } else
                widget->setHeight(((LayoutElement) *it).size.height());
        }

        // Set width
        if (widget->xConstraint() == FixedConstraint)
            widget->setWidth(((LayoutElement) *it).size.width());
        else if (widget->xConstraint() & ExpandPolicy)
            widget->setWidth(width());
        else if (widget->width() < width() && (widget->xConstraint() & GrowPolicy))
            widget->setWidth(width());
        else if (widget->width() >= width() && (widget->xConstraint() & ShrinkPolicy))
            widget->setWidth(width());

        // Set top-left using alignment.
        int x = 0;
        if (_alignment & Alignment::Right)
            x = (width() - widget->width());
        else if (_alignment & Alignment::Center)
            x = (width() - widget->width()) / 2;

        widget->moveTo(x, currentY);
        currentY += widget->height() + spacing();

        // Set neighbours
        if (it == itLast)
        {
            if (_keyNavChildrenFirst)
                down = (*list.begin()).widget;
            else
                down = getNeighbour(Down);
        } else
        {
            itNext = it;
            down = ((LayoutElement) *(++itNext)).widget;
        }

        widget->setNeighbours(up, down, left, right);
        up = widget;
    }

    _modified = false;
}

} /* namespace ilixi */
