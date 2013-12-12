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

#include <ui/ComboBox.h>
#include <ui/Dialog.h>
#include <ui/VBoxLayout.h>
#include <ui/RadioButton.h>
#include <ui/ScrollArea.h>
#include <ui/VBoxLayout.h>
#include <graphics/Painter.h>
#include <core/Logger.h>
#include <sigc++/bind.h>
#include <algorithm>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_COMBOBOX, "ilixi/ui/ComboBox", "ComboBox");

ComboBox::ComboBox(const std::string& title, Widget* parent)
        : Widget(parent),
          TextBase("", this),
          _dialogTitle(title),
          _selectedIndex(0)
{
    ILOG_TRACE_W(ILX_COMBOBOX);
    setInputMethod(KeyPointer);
    setConstraints(ExpandingConstraint, FixedConstraint);

    _down = new Icon(StyleHint::Down);
    addChild(_down);
}

ComboBox::ComboBox(const std::string& title, const StringList& items, Widget* parent)
        : Widget(parent),
          TextBase("", parent),
          _dialogTitle(title)
{
    ILOG_TRACE_W(ILX_COMBOBOX);
    setInputMethod(KeyPointer);
    setConstraints(ExpandingConstraint, FixedConstraint);

    _down = new Icon(StyleHint::Down);
    addChild(_down);

    addItems(items);
}

ComboBox::~ComboBox()
{
    ILOG_TRACE_W(ILX_COMBOBOX);
}

Size
ComboBox::preferredSize() const
{
    ILOG_TRACE_W(ILX_COMBOBOX);
    Size s = textExtents();
    return Size(s.width() + stylist()->defaultParameter(StyleHint::LineInputLR) + s.height(), s.height() + stylist()->defaultParameter(StyleHint::LineInputTB));
}

unsigned int
ComboBox::selectedIndex() const
{
    return _selectedIndex;
}

std::string
ComboBox::selectedItem() const
{
    if (_items.size())
        return _items.at(_selectedIndex);
    return "";
}

std::string
ComboBox::item(unsigned int index) const
{
    if (index < _items.size())
        return _items.at(index);
    return "";
}

void
ComboBox::addItem(const std::string& item)
{
    ILOG_TRACE_W(ILX_COMBOBOX);
    if (text().empty())
    {
        setText(item);
        _selectedIndex = _items.size();
    }
    _items.push_back(item);
}

void
ComboBox::addItems(const StringList& items)
{
    ILOG_TRACE_W(ILX_COMBOBOX);
    for (StringList::const_iterator it = items.begin(); it != items.end(); ++it)
    {
        if (text().empty())
        {
            setText(*it);
            _selectedIndex = _items.size();
        }

        _items.push_back(*it);
    }
}

void
ComboBox::setSelected(unsigned int index)
{
    ILOG_TRACE_W(ILX_COMBOBOX);
    if (index < _items.size())
    {
        _selectedIndex = index;
        setText(_items.at(_selectedIndex));
        sigItemChanged(_selectedIndex);
        sigSelectionChanged();
    }
}

void
ComboBox::setSelectedItem(const std::string& item)
{
    int i = 0;
    for (StringVector::const_iterator it = _items.begin(); it != _items.end(); ++it, ++i)
    {
        if (*it == item)
            setSelected(i);
    }
}

void
ComboBox::keyUpEvent(const KeyEvent& keyEvent)
{
    ILOG_TRACE_W(ILX_COMBOBOX);
    if (keyEvent.keySymbol == DIKS_CURSOR_UP)
        setSelected(_selectedIndex - 1);
    else if (keyEvent.keySymbol == DIKS_CURSOR_DOWN)
        setSelected(_selectedIndex + 1);
}

void
ComboBox::pointerButtonUpEvent(const PointerEvent& mouseEvent)
{
    ILOG_TRACE_W(ILX_COMBOBOX);
    if (_items.size())
    {
        update();
        initDialog();
    }
}

void
ComboBox::pointerWheelEvent(const PointerEvent& event)
{
    ILOG_TRACE_W(ILX_COMBOBOX);
    if (event.wheelStep < 0)
        setSelected(_selectedIndex + 1);
    else
        setSelected(_selectedIndex - 1);
}

void
ComboBox::focusInEvent()
{
    ILOG_TRACE_W(ILX_COMBOBOX);
    update();
}

void
ComboBox::focusOutEvent()
{
    ILOG_TRACE_W(ILX_COMBOBOX);
    update();
}

void
ComboBox::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    stylist()->drawComboBox(&p, this);
    p.end();
}

void
ComboBox::updateSelected(int index)
{
    ILOG_TRACE_W(ILX_COMBOBOX);
    if (index != _selectedIndex)
    {
        _selectedIndex = index;
        setText(item(_selectedIndex));
        if (_dialog)
            _dialog->finish(0);
        releaseDialog();
        sigItemChanged(_selectedIndex);
        sigSelectionChanged();
    }
}

void
ComboBox::updateTextBaseGeometry()
{
    ILOG_TRACE_W(ILX_COMBOBOX);
    int x = stylist()->defaultParameter(StyleHint::LineInputLeft);
    int y = stylist()->defaultParameter(StyleHint::LineInputTop);
    int iconOffset = (height() - 16) / 2;
    _down->setGeometry(width() - height() + iconOffset, iconOffset, 16, 16);

    _layout.setBounds(x, y, width() - x - height(), height() - stylist()->defaultParameter(StyleHint::LineInputTB));
    _layout.doLayout(font());
}

void
ComboBox::initDialog()
{
    ILOG_TRACE_W(ILX_COMBOBOX);
    _dialog = new Dialog(_dialogTitle, Dialog::CancelButtonOption);
    _dialog->setLayout(new VBoxLayout());
    _vlayout = new VBoxLayout();
    ScrollArea* scrollArea = new ScrollArea();
    scrollArea->setContent(_vlayout);
    _dialog->addWidget(scrollArea);
    _dialog->sigRejected.connect(sigc::mem_fun(this, &ComboBox::releaseDialog));

    int i = 0;
    for (StringVector::const_iterator it = _items.begin(); it != _items.end(); ++it, ++i)
    {
        RadioButton* rb = new RadioButton(*it);
        _vlayout->addWidget(rb);
        if (i == _selectedIndex)
            rb->setChecked();
        rb->sigClicked.connect(sigc::bind<int>(sigc::mem_fun(this, &ComboBox::updateSelected), i));
    }

    _dialog->execute();
}

void
ComboBox::releaseDialog()
{
    if (_dialog)
        delete _dialog;
}

Font*
ComboBox::defaultFont() const
{
    return stylist()->defaultFont(StyleHint::InputFont);
}

} /* namespace ilixi */
