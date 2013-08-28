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

#ifndef ILIXI_GRIDVIEW_H_
#define ILIXI_GRIDVIEW_H_

#include <ui/Widget.h>

namespace ilixi
{

class GridLayout;
class ScrollArea;

//! A container widget with a ScrollArea and a GridLayout.
class GridView : public Widget
{
public:
    /*!
     * Constructor.
     */
    GridView(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~GridView();

    Size
    preferredSize() const;

    /*!
     * Adds widget to internal layout.
     */
    void
    addItem(Widget* item);

    /*!
     * Removes all widgets from layout.
     */
    void
    clear();

    /*!
     * Returns number of widgets in layout.
     */
    unsigned int
    count() const;

    /*!
     * Returns last focused item.
     */
    Widget*
    currentItem() const;

    /*!
     * Returns the index of last focused item.
     */
    unsigned int
    currentIndex() const;

    /*!
     * Returns whether listbox draws a frame.
     */
    bool
    drawFrame() const;

    /*!
     * Returns the index for given item.
     */
    int
    itemIndex(Widget* item);

    /*!
     * Returns the widget at given index.
     *
     * Returns NULL if there is no item at index.
     */
    Widget*
    itemAtIndex(unsigned int index);

    /*!
     * Inserts a widget to layout at given index.
     */
    void
    insertItem(unsigned int index, Widget* item);

    /*!
     * Removes a given widget from layout.
     */
    bool
    removeItem(Widget* item);

    /*!
     * Removes widget from layout at given index.
     */
    bool
    removeItem(unsigned int index);

    /*!
     * Returns number of columns.
     */
    unsigned int
    columns() const;

    /*!
     * Returns number of rows.
     */
    unsigned int
    rows() const;

    /*!
     * Scrolls to given item at index.
     *
     * Setting current item with this method will not emit signals.
     */
    void
    setCurrentItem(unsigned int index);

    /*!
     * Scrolls to given item.
     *
     * Setting current item with this method will not emit signals.
     */
    void
    setCurrentItem(Widget* item);

    /*!
     * Sets whether frame is drawn.
     */
    void
    setDrawFrame(bool drawFrame);

    /*!
     * Sets whether to use thumb or scrollbar.
     * @param useThumbs if true do not use a scrollbar.
     */
    void
    setUseThumbs(bool useThumbs);

    /*!
     * Sets number of rows and columns.
     */
    void
    setGridSize(unsigned int rows, unsigned int cols);

    /*!
     * Sets space between children.
     */
    void
    setLayoutSpacing(int spacing);


    /*!
     * This signal is emitted when current item index is changed.
     *
     * Signal will pass old index and new index.
     */
    sigc::signal<void, unsigned int, unsigned int> sigIndexChanged;

    /*!
     * This signal is emitted when current item is in PressedState.
     */
    sigc::signal<void, Widget*> sigItemClicked;

    /*!
     * This signal is emitted when current item is in FocusedState.
     */
    sigc::signal<void, Widget*> sigItemSelected;

protected:
    void
    compose(const PaintEvent& event);

private:
    //! This holds internal layout.
    ScrollArea* _scrollArea;
    //! This is the internal grid layout.
    GridLayout* _layout;
    //! Index of current item.
    unsigned int _currentIndex;
    //! Points to current/last focused item.
    Widget* _currentItem;
    //! List of items in layout, stored for convenience.
    WidgetList _items;

    //! Sets geometry of ScrollArea.
    void
    updateGridViewGeometry();

    //! This method is called when an item's state changes.
    void
    trackItem(Widget* item, WidgetState state);

};

} /* namespace ilixi */
#endif /* ILIXI_GRIDVIEW_H_ */
