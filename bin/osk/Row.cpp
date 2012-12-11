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
    _box->addWidget(key);
    _keys.push_back(key);
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
