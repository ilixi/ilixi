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

#ifndef ILIXI_GRIDVIEW_H_
#define ILIXI_GRIDVIEW_H_

#include <ui/Widget.h>

namespace ilixi
{

class GridLayout;
class ScrollArea;

/*!
 *
 */
class GridView : public Widget
{
public:
    GridView(Widget* parent = 0);

    virtual
    ~GridView();

    virtual Size
    preferredSize() const;

    void
    addItem(Widget* item);

    void
    clear();

    unsigned int
    count() const;

    Widget*
    currentItem() const;

    unsigned int
    currentIndex() const;

    bool
    drawFrame() const;

    int
    itemIndex(Widget* item);

    Widget*
    itemAtIndex(unsigned int index);

    void
    insertItem(unsigned int index, Widget* item);

    bool
    removeItem(Widget* item);

    bool
    removeItem(unsigned int index);

    void
    setCurrentItem(unsigned int index);

    void
    setCurrentItem(Widget* item);

    void
    setDrawFrame(bool drawFrame);

    void
    setGridSize(unsigned int rows, unsigned int cols);

    sigc::signal<void, unsigned int, unsigned int> sigItemChanged;
    sigc::signal<void, unsigned int> sigIndexChanged;
    sigc::signal<void, Widget*> sigItemClicked;
    sigc::signal<void, Widget*> sigItemSelected;

protected:
    void
    compose(const PaintEvent& event);

private:
    ScrollArea* _scrollArea;
    GridLayout* _layout;

    bool _drawFrame;

    unsigned int _currentIndex;
    Widget* _currentItem;

    WidgetList _items;

    void
    updateGridViewGeometry();

    void
    trackItem(Widget* item, WidgetState state);

};

} /* namespace ilixi */
#endif /* ILIXI_GRIDVIEW_H_ */
