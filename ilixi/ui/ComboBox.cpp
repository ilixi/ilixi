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

#include <ui/ComboBox.h>
#include <ui/Dialog.h>
#include <ui/VBoxLayout.h>
#include <ui/RadioButton.h>
#include <ui/ScrollArea.h>
#include <ui/VBoxLayout.h>
#include <graphics/Painter.h>
#include <sigc++/bind.h>
#include <algorithm>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_COMBOBOX, "ilixi/ui/ComboBox", "ComboBox");

ComboBox::ComboBox(const std::string& title, Widget* parent)
        : Widget(parent),
          TextBase(title, this)
{
    setInputMethod(KeyAndPointerInput);
    setConstraints(MinimumConstraint, FixedConstraint);

    _vlayout = new VBoxLayout();

    _scrollArea = new ScrollArea();
    _scrollArea->setContent(_vlayout);

    _dialog = new Dialog(title, Dialog::CancelButtonOption);
    _dialog->setLayout(new VBoxLayout());
    _dialog->addWidget(_scrollArea);
    _dialog->sigFinished.connect(
            sigc::mem_fun(this, &ComboBox::updateSelected));
}

ComboBox::ComboBox(const std::string& title, const StringList& items,
                   Widget* parent)
        : TextBase(title, parent)
{
    setInputMethod(KeyAndPointerInput);
    setConstraints(MinimumConstraint, FixedConstraint);

    _dialog = new Dialog(title, Dialog::CancelButtonOption);
    _dialog->sigFinished.connect(
            sigc::mem_fun(this, &ComboBox::updateSelected));

    addItems(items);
    setSelected(_selectedIndex);
}

ComboBox::~ComboBox()
{
    delete _dialog;
}

Size
ComboBox::preferredSize() const
{
    Size s = textExtents();
//    return Size(
//            s.width()
//                    + 2
//                            * std::max(
//                                    stylist()->defaultParameter(
//                                            StyleHint::BorderWidth),
//                                    stylist()->defaultParameter(
//                                            StyleHint::ComboBoxRadius))
//                    + stylist()->defaultParameter(
//                            StyleHint::ComboBoxButtonWidth),
//            s.height()
//                    + 2 * stylist()->defaultParameter(StyleHint::BorderWidth));
    return Size();
}

unsigned int
ComboBox::selectedIndex() const
{
    return _selectedIndex;
}

std::string
ComboBox::selectedItem() const
{
    return _items.at(_selectedIndex)->text();
}

std::string
ComboBox::item(unsigned int index) const
{
    if (index < _items.size())
        return _items.at(index)->text();
    return "";
}

void
ComboBox::addItem(const std::string& item)
{
    _selectedIndex = _items.size();
    setText(item);
    RadioButton* rb = new RadioButton(item);
    rb->toggleChecked();
    rb->sigClicked.connect(
            sigc::bind<int>(sigc::mem_fun(_dialog, &Dialog::finish),
                            _selectedIndex));

    _items.push_back(rb);
    _vlayout->addWidget(rb);
}

void
ComboBox::addItems(const StringList& items)
{
    _selectedIndex = _items.size();
    for (StringList::const_iterator it = items.begin(); it != items.end(); ++it)
    {
        RadioButton* rb = new RadioButton(*it);

        rb->sigClicked.connect(
                sigc::bind<int>(sigc::mem_fun(_dialog, &Dialog::finish),
                                _items.size()));

        _items.push_back(rb);
        _vlayout->addWidget(_items.back());
    }
    setText(_items.at(_selectedIndex)->text());
}

void
ComboBox::setSelected(unsigned int index)
{
    if (index < _items.size())
    {
        _selectedIndex = index;
        _items[index]->toggleChecked();
        setText(_items.at(_selectedIndex)->text());
    }
}

void
ComboBox::keyUpEvent(const KeyEvent& keyEvent)
{
    if (keyEvent.keySymbol == DIKS_CURSOR_UP)
        setSelected(_selectedIndex - 1);
    else if (keyEvent.keySymbol == DIKS_CURSOR_DOWN)
        setSelected(_selectedIndex + 1);
}

void
ComboBox::pointerButtonUpEvent(const PointerEvent& mouseEvent)
{
    if (_items.size())
    {
        update();
        _dialog->execute();
    }
}

void
ComboBox::pointerWheelEvent(const PointerEvent& event)
{
    if (event.wheelStep < 0)
        setSelected(_selectedIndex + 1);
    else
        setSelected(_selectedIndex - 1);
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
    if (index != _selectedIndex)
    {
        _selectedIndex = index;
        setText(item(_selectedIndex));
    }
}

void
ComboBox::updateTextBaseGeometry()
{
//    int border = std::max(
//            stylist()->defaultParameter(StyleHint::BorderWidth),
//            stylist()->defaultParameter(StyleHint::ComboBoxRadius));
//    int x = border;
//    int y = stylist()->defaultParameter(StyleHint::BorderWidth);
//
//    _layout.setBounds(
//            x,
//            y,
//            width() - x
//                    - stylist()->defaultParameter(
//                            StyleHint::ComboBoxButtonWidth),
//            height() - 2 * stylist()->defaultParameter(StyleHint::BorderWidth));
    _layout.doLayout(font());
}

Font*
ComboBox::defaultFont() const
{
    return stylist()->defaultFont(StyleHint::InputFont);
}

} /* namespace ilixi */
