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

#include <ui/HBoxLayout.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_HBOX, "ilixi/ui/HBox", "HBox");

HBoxLayout::HBoxLayout(Widget* parent)
        : LayoutBase(parent),
          _alignment(Alignment::Top)
{
    ILOG_TRACE_W(ILX_HBOX);
    setConstraints(MinimumConstraint, MinimumConstraint);
}

HBoxLayout::~HBoxLayout()
{
    ILOG_TRACE_W(ILX_HBOX);
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

bool
HBoxLayout::insertWidget(unsigned int column, Widget* widget)
{
    if (insertChild(widget, column))
    {
        doLayout();
        return true;
    }
    return false;
}

int
HBoxLayout::heightForWidth(int width) const
{
    ILOG_TRACE_W(ILX_HBOX);
    if (!_children.size())
        return -1;

    ElementList list, l;    // temporary widget lists
    LayoutElement e;        // Current element
    int maxH = 0;           // maximum height.
    int h = 0;              // current element's height
    int w = 0;              // current element's width.
    int h4w = 0;

    // init. temp tile list
    for (Widget::WidgetListConstIterator it = _children.begin(); it != _children.end(); ++it)
    {
        e.widget = ((Widget*) *it);
        if (e.widget->visible() && e.widget->xConstraint() != IgnoredConstraint)
        {
            e.size = e.widget->preferredSize();
            list.push_back(e);
        }
    }

    if (l.size() == 0)
        return -1;

    // calculate a temp width for each list element.
    // later calc. h4w of each element using temp width.
    l = list;
    int availableSpace = width - ((l.size() - 1) * spacing());
    int average = availableSpace / l.size();

    //***********************************************************
    // FixedConstraint
    //***********************************************************
    int usedSpace = 0;
    ElementList::iterator it = l.begin();
    while (it != l.end())
    {
        if (((LayoutElement) *it).widget->xConstraint() == FixedConstraint && ((LayoutElement) *it).widget->minWidth() < 0 && ((LayoutElement) *it).widget->maxWidth() < 0)
        {
            usedSpace += ((LayoutElement) *it).size.width();
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
        if (average < ((LayoutElement) *it).widget->minWidth())
        {
            usedSpace += ((LayoutElement) *it).widget->minWidth();
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
        if (average > ((LayoutElement) *it).widget->maxWidth() && ((LayoutElement) *it).widget->maxWidth() > 0)
        {
            usedSpace += ((LayoutElement) *it).widget->maxWidth();
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
        if (average < ((LayoutElement) *it).size.width() && !(((LayoutElement) *it).widget->xConstraint() & ShrinkPolicy))
        {
            usedSpace += ((LayoutElement) *it).size.width();
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
        if (average > ((LayoutElement) *it).size.width() && !(((LayoutElement) *it).widget->xConstraint() & GrowPolicy))
        {
            usedSpace += ((LayoutElement) *it).widget->width();
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
        if (((LayoutElement) *it).widget->xConstraint() & ExpandPolicy)
            ++expanding;
        else
        {
            if (((LayoutElement) *it).widget->minWidth() > 0 && average > ((LayoutElement) *it).widget->minWidth())
            {
                expandSpace += average - ((LayoutElement) *it).widget->minWidth();
                ((LayoutElement) *it).size.setWidth(((LayoutElement) *it).widget->minWidth());
            } else if (average > ((LayoutElement) *it).size.width())
                expandSpace += average - ((LayoutElement) *it).size.width();
        }
        ++it;
    }

    if (expandSpace && expanding)
        expandAverage = expandSpace / expanding;

    //***********************************************************
    // calc height
    //***********************************************************
    int artifact = availableSpace - average * l.size();
    Widget* widget;
    for (ElementList::iterator it = list.begin(), end = list.end(), itLast = (++list.rbegin()).base(); it != end; ++it)
    {
        widget = ((LayoutElement) *it).widget;

        // Set w for each element
        if (expanding)
        {
            if (widget->xConstraint() & ExpandPolicy)
            {
                if (artifact)
                {
                    w = average + expandAverage + artifact;
                    artifact = 0;
                } else
                    w = average + expandAverage;
            } else if ((widget->xConstraint() & ShrinkPolicy) && average < ((LayoutElement) *it).size.width())
                w = average;
            else
                w = ((LayoutElement) *it).size.width();
        } else
        {
            if (widget->minWidth() > 0 && average < widget->minWidth())
                w = widget->minWidth();
            else if (widget->maxWidth() > 0 && average > widget->maxWidth())
                w = widget->maxWidth();
            else if ((widget->xConstraint() & ShrinkPolicy) && ((LayoutElement) *it).size.width() > average)
                w = average;
            else if ((widget->xConstraint() & GrowPolicy) && ((LayoutElement) *it).size.width() < average)
            {
                if (artifact)
                {
                    w = average + artifact;
                    artifact = 0;
                } else
                    w = average;
            } else
                w = ((LayoutElement) *it).size.width();
        }

        // Set h using w or h4w
        if (widget->yConstraint() == FixedConstraint)
            h = ((LayoutElement) *it).size.height();
        else
        {
            h4w = widget->heightForWidth(w);
            if (h4w)
            {
                if (widget->minHeight() && h4w < widget->minHeight())
                    h4w = widget->minHeight();
                else if (widget->maxHeight() && h4w > widget->maxHeight())
                    h4w = widget->maxHeight();
                h = h4w;
            } else
                h = ((LayoutElement) *it).size.height();
        }

        if (h > maxH)
            maxH = h;
    }

    return maxH;
}

Size
HBoxLayout::preferredSize() const
{
    ILOG_TRACE_W(ILX_HBOX);
    if (!_children.size())
        return Size(50, 50);

    LayoutElement e;    // Current element
    int w = 0;          // total width
    int h = 0;          // max. height
    int cw = 0;         // current widget's width.
    int ch = 0;         // current widget's height.

    for (WidgetList::const_iterator it = _children.begin(); it != _children.end(); ++it)
    {
        e.widget = ((Widget*) *it);
        if (e.widget->visible() && e.widget->xConstraint() != IgnoredConstraint)
        {
            e.size = e.widget->preferredSize();
            cw = e.size.width();
            ch = e.size.height();

            // satisfy min-max width but check for fixed cons. first
            if (e.widget->minWidth() > 0 && cw < e.widget->minWidth())
                cw = e.widget->minWidth();
            else if (e.widget->maxWidth() > 0 && cw > e.widget->maxWidth())
                cw = e.widget->maxWidth();

            // satisfy min-max height
            if (e.widget->minHeight() > 0 && ch < e.widget->minHeight())
                ch = e.widget->minHeight();
            else if (e.widget->maxHeight() > 0 && ch > e.widget->maxHeight())
                ch = e.widget->maxHeight();

            if (ch > h)
                h = ch;

            w += cw + spacing();
        }
    }
    ILOG_DEBUG(ILX_HBOX, " -> Size(%d, %d)\n", w - spacing(), h);
    return Size(w - spacing(), h);
}

void
HBoxLayout::tile()
{
    ILOG_TRACE_W(ILX_HBOX);
    ElementList list;
    LayoutElement e;
    for (Widget::WidgetListConstIterator it = _children.begin(); it != _children.end(); ++it)
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
        if (((LayoutElement) *it).widget->xConstraint() == FixedConstraint && ((LayoutElement) *it).widget->minWidth() < 0 && ((LayoutElement) *it).widget->maxWidth() < 0)
        {
            usedSpace += ((LayoutElement) *it).size.width();
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
        if (average < ((LayoutElement) *it).widget->minWidth())
        {
            usedSpace += ((LayoutElement) *it).widget->minWidth();
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
        if (average > ((LayoutElement) *it).widget->maxWidth() && ((LayoutElement) *it).widget->maxWidth() > 0)
        {
            usedSpace += ((LayoutElement) *it).widget->maxWidth();
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
        if (average < ((LayoutElement) *it).size.width() && !(((LayoutElement) *it).widget->xConstraint() & ShrinkPolicy))
        {
            usedSpace += ((LayoutElement) *it).size.width();
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
        if (average > ((LayoutElement) *it).size.width() && !(((LayoutElement) *it).widget->xConstraint() & GrowPolicy))
        {
            usedSpace += ((LayoutElement) *it).widget->width();
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
        if (((LayoutElement) *it).widget->xConstraint() & ExpandPolicy)
            ++expanding;
        else
        {
            if (((LayoutElement) *it).widget->minWidth() > 0 && average > ((LayoutElement) *it).widget->minWidth())
            {
                expandSpace += average - ((LayoutElement) *it).widget->minWidth();
                ((LayoutElement) *it).size.setWidth(((LayoutElement) *it).widget->minWidth());
            } else if (average > ((LayoutElement) *it).size.width())
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
    Widget* left = _keyNavChildrenFirst ? (*list.end()).widget : getNeighbour(Left);
    Widget* right = getNeighbour(Right);
    Widget* up = getNeighbour(Up);
    Widget* down = getNeighbour(Down);
    ElementList::iterator itNext, itLast;
    for (ElementList::iterator it = list.begin(), end = list.end(), itLast = (++list.rbegin()).base(); it != end; ++it)
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
                } else
                    widget->setWidth(average + expandAverage);
            } else if ((widget->xConstraint() & ShrinkPolicy) && average < ((LayoutElement) *it).size.width())
                widget->setWidth(average);
            else
                widget->setWidth(((LayoutElement) *it).size.width());
        } else
        {
            if (widget->minWidth() > 0 || widget->maxWidth() > 0)
                widget->setWidth(average);
            else if ((widget->xConstraint() & ShrinkPolicy) && ((LayoutElement) *it).size.width() > average)
                widget->setWidth(average);
            else if ((widget->xConstraint() & GrowPolicy) && ((LayoutElement) *it).size.width() < average)
            {
                if (artifact)
                {
                    widget->setWidth(average + artifact);
                    artifact = 0;
                } else
                    widget->setWidth(average);
            } else
                widget->setWidth(((LayoutElement) *it).size.width());
        }

        // Set height
        if (widget->yConstraint() == FixedConstraint)
            widget->setHeight(((LayoutElement) *it).size.height());
        else if (widget->yConstraint() & ExpandPolicy)
            widget->setHeight(height());
        else if (widget->height() < height() && (widget->yConstraint() & GrowPolicy))
            widget->setHeight(height());
        else if (widget->height() >= height() && (widget->yConstraint() & ShrinkPolicy))
            widget->setHeight(height());

        // check if widget provides a height for width, if not use
        // height for hbox instead.
//        int h4w = widget->heightForWidth(widget->width());
//        if (h4w > 0)
//        {
//            ILOG_DEBUG(ILX_HBOX, "h4w: %d!\n", h4w);
//            // check grow-shrink for height
//            if (((LayoutElement) *it).size.height() < h4w
//                    && (widget->yConstraint() & GrowPolicy))
//                widget->setHeight(h4w);
//            else if (((LayoutElement) *it).size.height() >= h4w
//                    && (widget->yConstraint() & ShrinkPolicy))
//                widget->setHeight(h4w);
//        }

        // Set top-left using alignment.
        int y = 0;
        if (_alignment == Alignment::Bottom)
            y = (height() - widget->height());
        else if (_alignment == Alignment::Middle)
            y = (height() - widget->height()) / 2;

        widget->moveTo(currentX, y);
        currentX += widget->width() + spacing();

        // Set neighbours
        if (it == itLast)
        {
            if (_keyNavChildrenFirst)
                right = (*list.begin()).widget;
            else
                right = getNeighbour(Right);
        } else
        {
            itNext = it;
            right = ((LayoutElement) *(++itNext)).widget;
        }

        widget->setNeighbours(up, down, left, right);
        left = widget;
    }

    _modified = false;
}

} /* namespace ilixi */
