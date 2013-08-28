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

#include <ui/GridView.h>
#include <ui/GridLayout.h>
#include <ui/ScrollArea.h>
#include <graphics/Painter.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_GRIDVIEW, "ilixi/ui/GridView", "GridView");

GridView::GridView(Widget* parent)
        : Widget(parent),
          _scrollArea(NULL),
          _layout(NULL),
          _currentIndex(0),
          _currentItem(NULL)
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
    setInputMethod(PointerPassthrough);
    _scrollArea = new ScrollArea();
    addChild(_scrollArea);

    _layout = new GridLayout(2, 2);
    _layout->setKeyNavChildrenFirst(true);
    _scrollArea->setContent(_layout);

    sigGeometryUpdated.connect(sigc::mem_fun(this, &GridView::updateGridViewGeometry));
}

GridView::~GridView()
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
}

Size
GridView::preferredSize() const
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
    return _scrollArea->preferredSize();
}

void
GridView::addItem(Widget* item)
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
    if (_layout->addWidget(item))
    {
        _items.push_back(item);
        item->sigStateChanged.connect(sigc::mem_fun(this, &GridView::trackItem));
    }
}

void
GridView::clear()
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
    _items.clear();
    _layout->clear();
}

unsigned int
GridView::count() const
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
    return _layout->count();
}

Widget*
GridView::currentItem() const
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
    return _currentItem;
}

unsigned int
GridView::currentIndex() const
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
    return _currentIndex;
}

bool
GridView::drawFrame() const
{
    return _scrollArea->drawFrame();
}

int
GridView::itemIndex(Widget* item)
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
    int i = 0;
    for (WidgetList::iterator it = _items.begin(); it != _items.end(); ++it, ++i)
        if (*it == item)
            return i;
    return -1;
}

Widget*
GridView::itemAtIndex(unsigned int index)
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
    if (index > _items.size())
        return NULL;
    int i = 0;
    for (WidgetList::iterator it = _items.begin(); it != _items.end(); ++it, ++i)
        if (i == index)
            return *it;
    return NULL;
}

void
GridView::insertItem(unsigned int index, Widget* item)
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
    _layout->addWidget(item, index / _layout->columns(), index % _layout->columns());
}

bool
GridView::removeItem(Widget* item)
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
    if (item == _currentItem)
        setCurrentItem(_currentIndex + 1 > _items.size() ? 0 : _currentIndex + 1);
    return _layout->removeWidget(item);
}

bool
GridView::removeItem(unsigned int index)
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
    Widget* widget = itemAtIndex(index);
    if (widget)
    {
        _layout->removeWidget(widget);
        return true;
    }
    return false;
}

unsigned int
GridView::columns() const
{
    return _layout->columns();
}

unsigned int
GridView::rows() const
{
    return _layout->rows();
}

void
GridView::setCurrentItem(unsigned int index)
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
    if (_currentIndex != index && index < _items.size())
    {
        _currentIndex = index;
        _currentItem = itemAtIndex(_currentIndex);
        _scrollArea->scrollTo(_currentItem);
    }
}

void
GridView::setCurrentItem(Widget* item)
{
    ILOG_TRACE_W(ILX_GRIDVIEW);
    if (_currentItem != item && _layout->isChild(item))
    {
        _currentItem = item;
        _scrollArea->scrollTo(_currentItem);
    }
}

void
GridView::setDrawFrame(bool drawFrame)
{
    _scrollArea->setDrawFrame(drawFrame);
}

void
GridView::setUseThumbs(bool useThumbs)
{
    _scrollArea->setUseThumbs(useThumbs);
}

void
GridView::setGridSize(unsigned int rows, unsigned int cols)
{
    if (rows != _layout->rows() || cols != _layout->columns())
    {
        _layout = new GridLayout(rows, cols);
        _scrollArea->setContent(_layout);
        _layout->setKeyNavChildrenFirst(true);
    }
}

void
GridView::setLayoutSpacing(int spacing)
{
    _layout->setSpacing(spacing);
}

void
GridView::compose(const PaintEvent& event)
{
}

void
GridView::updateGridViewGeometry()
{
    _scrollArea->setGeometry(0, 0, width(), height());
}

void
GridView::trackItem(Widget* item, WidgetState state)
{
    if (_scrollArea->pressed())
        return;

    if (state & FocusedState)
    {
        int oldIndex = _currentIndex;
        setCurrentItem(item);
        if (_currentIndex != oldIndex)
            sigIndexChanged(oldIndex, _currentIndex);
        sigItemSelected(item);
    }

    if (state & PressedState)
        sigItemClicked(item);
}

} /* namespace ilixi */
