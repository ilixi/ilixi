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

#ifndef ILIXI_LISTBOX_H_
#define ILIXI_LISTBOX_H_

#include <ui/Widget.h>

namespace ilixi
{
class LayoutBase;
class ScrollArea;

//! A container widget with a ScrollArea and a horizontal or vertical layout.
class ListBox : public Widget
{
public:
    /*!
     * Contructor.
     *
     * Creates a listbox with vertical layout.
     */
    ListBox(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~ListBox();

    virtual Size
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
     * Returns current orientation, i.e. horizontal or vertical.
     */
    Orientation
    orientation() const;

    /*!
     * Scrolls to given item at index.
     *
     * Setting current item with this method will not emit sigItemSelected.
     */
    void
    setCurrentItem(unsigned int index);

    /*!
     * Scrolls to given item.
     *
     * Setting current item with this method will not emit sigItemSelected.
     */
    void
    setCurrentItem(Widget* item);

    /*!
     * Sets orientation.
     *
     * If orientation is changed it will remove existing layout and all child items.
     */
    void
    setOrientation(Orientation orientation);

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
     * Set spacing for internal layout.
     */
    void
    setSpacing(int spacing);

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
    virtual void
    compose(const PaintEvent& event);

private:
    //! This property stores current orientation.
    Orientation _orientation;
    //! This holds internal layout.
    ScrollArea* _scrollArea;
    //! This is the internal layout.
    LayoutBase* _layout;
    //! Index of current item.
    unsigned int _currentIndex;
    //! Points to current/last focused item.
    Widget* _currentItem;
    //! List of items in layout, stored for convenience.
    WidgetList _items;

    //! Sets geometry of ScrollArea.
    void
    updateListBoxGeometry();

    //! This method is called when an item's state changes.
    void
    trackItem(Widget* item, WidgetState state);
};

} /* namespace ilixi */
#endif /* ILIXI_LISTBOX_H_ */
