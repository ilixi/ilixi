/*
 * Row.cpp
 *
 *  Created on: Aug 6, 2012
 *      Author: tarik
 */

#include "Row.h"

namespace ilixi
{

Row::Row(const std::string& id, Widget* parent)
        : Widget(parent),
          _id(id),
          _gap(0),
          _keyHeight(100)
{
    _box = new HBoxLayout();
    addChild(_box);
}

Row::~Row()
{
}

std::string
Row::id() const
{
    return _id;
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
    _box->addWidget(key);
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
Row::setRowSymbolIndex(unsigned char index)
{
    for (unsigned int i = 0; i < _keys.size(); ++i)
        _keys[i]->setSymbolState(index);
}

void
Row::updateRowGeometry()
{
    _box->setGeometry(_gap, 0, width() - 2 * _gap, height());
}

} /* namespace ilixi */
