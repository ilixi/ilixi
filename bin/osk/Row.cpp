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

#include "Row.h"

namespace ilixi
{

Row::Row(const std::string& id, Widget* parent)
        : Widget(parent),
          _xmlID(id),
          _gap(0),
          _keyHeight(100)
{
    setInputMethod(PointerInput);
    _box = new HBoxLayout();
    addChild(_box);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &Row::updateRowGeometry));
}

Row::~Row()
{
}

std::string
Row::xmlID() const
{
    return _xmlID;
}

unsigned int
Row::gap() const
{
    return _gap;
}

Key*
Row::key(const std::string& label)
{
    for (unsigned int i = 0; i < _keys.size(); ++i)
        if (_keys[i]->text() == label)
            return _keys[i];
    return NULL;
}

void
Row::addKey(Key* key)
{
    if (key != NULL)
    {
        _box->addWidget(key);
        _keys.push_back(key);
    }
}

void
Row::removeKey(Key* key)
{
    _box->removeWidget(key);
}

void
Row::setGap(int px)
{
    _gap = px;
}

unsigned int
Row::keyHeight() const
{
    return _keyHeight;
}

void
Row::setKeyHeight(unsigned int keyHeight)
{
    _keyHeight = keyHeight;
}

void
Row::setSymbolState(unsigned char index)
{
    for (unsigned int i = 0; i < _keys.size(); ++i)
        _keys[i]->setSymbolState(index);
}

void
Row::setKeyFont(Font* font)
{
    for (unsigned int i = 0; i < _keys.size(); ++i)
        _keys[i]->setFont(font);
}

void
Row::compose(const PaintEvent& event)
{
}

void
Row::updateRowGeometry()
{
    _box->setGeometry(_gap, 0, width() - 2 * _gap, height());
    _box->setNeighbours(getNeighbour(Up), getNeighbour(Down), getNeighbour(Left), getNeighbour(Right));
}

} /* namespace ilixi */
