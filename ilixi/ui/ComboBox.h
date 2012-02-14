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

#ifndef ILIXI_COMBOBOX_H_
#define ILIXI_COMBOBOX_H_

#include "ui/TextBase.h"
#include <vector>

namespace ilixi
{

  class Dialog;
  class RadioButton;
  class ScrollArea;
  class VBoxLayout;

  class ComboBox : public TextBase
  {
  public:
    typedef std::list<std::string> StringList;

    ComboBox(const std::string& title, Widget* parent = 0);

    ComboBox(const std::string& title, const StringList& items, Widget* parent =
        0);

    virtual
    ~ComboBox();

    virtual Size
    preferredSize() const;

    unsigned int
    selectedIndex() const;

    std::string
    selectedItem() const;

    std::string
    item(unsigned int index) const;

    void
    addItem(const std::string& item);

    void
    addItems(const StringList& items);

    void
    setSelected(unsigned int index);

  private:
    Dialog* _dialog;
    ScrollArea* _scrollArea;
    VBoxLayout* _vlayout;

    unsigned int _selectedIndex;

    std::vector<RadioButton*> _items;

    void
    updateSelected(int index);

    virtual void
    keyUpEvent(const KeyEvent& keyEvent);

    virtual void
    pointerButtonUpEvent(const PointerEvent& mouseEvent);

    virtual void
    pointerWheelEvent(const PointerEvent& event);

    virtual void
    compose();

    virtual void
    updateTextLayoutGeometry();

    virtual Font*
    defaultFont() const;
  };

} /* namespace ilixi */
#endif /* ILIXI_COMBOBOX_H_ */
