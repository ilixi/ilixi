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

#include <ui/ListBox.h>
#include <ui/HBoxLayout.h>
#include <ui/VBoxLayout.h>
#include <ui/ScrollArea.h>
#include <core/Logger.h>
#include <graphics/Painter.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_LISTBOX, "ilixi/ui/ListBox", "ListBox");

ListBox::ListBox(Widget* parent)
        : Widget(parent),
          _orientation(Vertical),
          _scrollArea(NULL),
          _layout(NULL),
          _currentIndex(-1),
          _currentItem(NULL)
{
    ILOG_TRACE_W(ILX_LISTBOX);
    setInputMethod(PointerPassthrough);
    _scrollArea = new ScrollArea();
    addChild(_scrollArea);

    _layout = new VBoxLayout();
    _layout->setKeyNavChildrenFirst(true);
    _scrollArea->setContent(_layout);

    sigGeometryUpdated.connect(sigc::mem_fun(this, &ListBox::updateListBoxGeometry));
}

ListBox::~ListBox()
{
    ILOG_TRACE_W(ILX_LISTBOX);
}

Size
ListBox::preferredSize() const
{
    return _scrollArea->preferredSize();
}

void
ListBox::addItem(Widget* item)
{
    ILOG_TRACE_W(ILX_LISTBOX);
    if (_layout->addWidget(item))
    {
        _items.push_back(item);
        item->sigStateChanged.connect(sigc::mem_fun(this, &ListBox::trackItem));
    }
}

void
ListBox::clear()
{
    ILOG_TRACE_W(ILX_LISTBOX);
    _layout->clear();
    _items.clear();
}

unsigned int
ListBox::count() const
{
    ILOG_TRACE_W(ILX_LISTBOX);
    return _layout->count();
}

Widget*
ListBox::currentItem() const
{
    ILOG_TRACE_W(ILX_LISTBOX);
    return _currentItem;
}

unsigned int
ListBox::currentIndex() const
{
    ILOG_TRACE_W(ILX_LISTBOX);
    return _currentIndex;
}

int
ListBox::itemIndex(Widget* item)
{
    ILOG_TRACE_W(ILX_LISTBOX);
    int i = 0;
    for (WidgetList::iterator it = _items.begin(); it != _items.end(); ++it, ++i)
        if (*it == item)
            return i;
    return -1;
}

Widget*
ListBox::itemAtIndex(unsigned int index)
{
    ILOG_TRACE_W(ILX_LISTBOX);
    if (index > _items.size())
        return NULL;
    int i = 0;
    for (WidgetList::iterator it = _items.begin(); it != _items.end(); ++it, ++i)
        if (i == index)
            return *it;
    return NULL;
}

void
ListBox::insertItem(unsigned int index, Widget* item)
{
    ILOG_TRACE_W(ILX_LISTBOX);
    if (_orientation == Horizontal)
        ((HBoxLayout*) _layout)->insertWidget(index, item);
    else
        ((VBoxLayout*) _layout)->insertWidget(index, item);
}

bool
ListBox::removeItem(Widget* item)
{
    ILOG_TRACE_W(ILX_LISTBOX);
    if (_layout->removeWidget(item))
    {
        for (WidgetListIterator it = _items.begin(); it != _items.end(); ++it)
        {
            if (*it == item)
            {
                _items.erase(it);
                break;
            }
        }

        setCurrentItem(_currentIndex > _items.size() ? 0 : _currentIndex - 1);

        return true;
    }
    return false;
}

bool
ListBox::removeItem(unsigned int index)
{
    ILOG_TRACE_W(ILX_LISTBOX);
    Widget* widget = itemAtIndex(index);
    if (widget)
    {
        _layout->removeWidget(widget);
        return true;
    }
    return false;
}

Orientation
ListBox::orientation() const
{
    return _orientation;
}

void
ListBox::setCurrentItem(unsigned int index)
{
    ILOG_TRACE_W(ILX_LISTBOX);
    if (_currentIndex != index && index < _items.size())
    {
        _currentItem = itemAtIndex(index);
        _scrollArea->scrollTo(_currentItem);

        int oldIndex = _currentIndex;
        _currentIndex = index;
        if (_currentIndex != oldIndex)
            sigIndexChanged(oldIndex, _currentIndex);
    }
}

void
ListBox::setCurrentItem(Widget* item)
{
    ILOG_TRACE_W(ILX_LISTBOX);
    if (_currentItem != item && _layout->isChild(item))
    {
        _currentItem = item;
        _scrollArea->scrollTo(_currentItem);

        int oldIndex = _currentIndex;
        _currentIndex = itemIndex(_currentItem);
        if (_currentIndex != oldIndex)
            sigIndexChanged(oldIndex, _currentIndex);
    }
}

void
ListBox::setOrientation(Orientation orientation)
{
    ILOG_TRACE_W(ILX_LISTBOX);
    if (_orientation != orientation)
    {
        _orientation = orientation;

        for (WidgetListIterator it = _items.begin(); it != _items.end(); ++it)
            _layout->removeWidget(*it, false);
        removeChild(_layout);

        if (_orientation == Vertical)
            _layout = new VBoxLayout();
        else
            _layout = new HBoxLayout();

        for (WidgetListIterator it = _items.begin(); it != _items.end(); ++it)
            _layout->addWidget(*it);
        _layout->setKeyNavChildrenFirst(true);
        addChild(_layout);
    }
}

bool
ListBox::drawFrame() const
{
    return _scrollArea->drawFrame();
}

void
ListBox::setDrawFrame(bool drawFrame)
{
    _scrollArea->setDrawFrame(drawFrame);
}

void
ListBox::setUseThumbs(bool useThumbs)
{
    _scrollArea->setUseThumbs(useThumbs);
}

void
ListBox::setSpacing(int spacing)
{
    _layout->setSpacing(spacing);
}

void
ListBox::compose(const PaintEvent& event)
{
}

void
ListBox::updateListBoxGeometry()
{
    _scrollArea->setGeometry(0, 0, width(), height());
    _scrollArea->setNeighbours(getNeighbour(Up), getNeighbour(Down), getNeighbour(Left), getNeighbour(Right));
}

void
ListBox::trackItem(Widget* item, WidgetState state)
{
    if (_scrollArea->pressed())
        return;

    if (state & FocusedState)
    {
        setCurrentItem(item);
        sigItemSelected(item);
    }

    if (state & PressedState)
        sigItemClicked(item);
}

} /* namespace ilixi */
