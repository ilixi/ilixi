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

#ifndef ILIXI_COMBOBOX_H_
#define ILIXI_COMBOBOX_H_

#include <ui/TextBase.h>
#include <ui/Icon.h>
#include <vector>

namespace ilixi
{

class Dialog;
class RadioButton;
class ScrollArea;
class VBoxLayout;

//! Provides a dialog with a list of options.
/*!
 * When clicked a combobox shows a list of options, text only, and allows selecting only one option.
 */
class ComboBox : public Widget, public TextBase
{
public:
    //! String list
    typedef std::list<std::string> StringList;

    /*!
     * Constructor
     */
    ComboBox(const std::string& title, Widget* parent = 0);

    /*!
     * Constructor
     */
    ComboBox(const std::string& title, const StringList& items, Widget* parent = 0);

    /*!
     * Destructor
     */
    virtual
    ~ComboBox();

    /*!
     * Returns an optimal size for combobox box
     */
    virtual Size
    preferredSize() const;

    /*!
     * Returns index of currently selected item.
     */
    unsigned int
    selectedIndex() const;

    /*!
     * Returns the text of selected item.
     */
    std::string
    selectedItem() const;

    /*!
     * Returns the text of item at given index.
     */
    std::string
    item(unsigned int index) const;

    /*!
     * Adds a new item to combobox.
     */
    void
    addItem(const std::string& item);

    /*!
     * Adds a list of new items to combobox.
     */
    void
    addItems(const StringList& items);

    /*!
     * Selects item at given index.
     */
    void
    setSelected(unsigned int index);

    /*!
     * Selects item with given text.
     */
    void
    setSelectedItem(const std::string& item);

    /*!
     * This signal is emitted when a new item is selected.
     *
     * Signal passes index of selected item.
     */
    sigc::signal<void, unsigned int> sigItemSelected;

protected:
    /*!
     * Changes selection if Up or Down cursor key is released.
     */
    virtual void
    keyUpEvent(const KeyEvent& keyEvent);

    /*!
     * Shows selection dialog when pointer is released.
     */
    virtual void
    pointerButtonUpEvent(const PointerEvent& mouseEvent);

    /*!
     * Changes selection when wheel is scrolled.
     */
    virtual void
    pointerWheelEvent(const PointerEvent& event);

    /*!
     * Just updates.
     */
    virtual void
    focusInEvent();

    /*!
     * Just updates.
     */
    virtual void
    focusOutEvent();

    virtual void
    compose(const PaintEvent& event);

private:
    //! This property stores title of selection dialog.
    std::string _dialogTitle;
    //! This property stores index of selected item.
    unsigned int _selectedIndex;

    //! This is the selection dialog.
    Dialog* _dialog;
    //! Layout for selection dialog.
    VBoxLayout* _vlayout;
    Icon* _down;
    typedef std::vector<std::string> StringVector;
    StringVector _items;

    void
    updateSelected(int index);

    virtual void
    updateTextBaseGeometry();

    void
    initDialog();

    void
    releaseDialog();

    virtual Font*
    defaultFont() const;
};

} /* namespace ilixi */
#endif /* ILIXI_COMBOBOX_H_ */
