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

#ifndef ROW_H_
#define ROW_H_

#include <ui/Widget.h>
#include <ui/HBoxLayout.h>
#include "Key.h"
#include <vector>

namespace ilixi
{

class Row : public Widget
{
    friend class Keyboard;
public:
    Row(const std::string& id, Widget* parent = 0);

    virtual
    ~Row();

    std::string
    xmlID() const;

    unsigned int
    gap() const;

    Key*
    key(const std::string& label);

    void
    addKey(Key* key);

    void
    removeKey(Key* key);

    void
    setGap(int px);

    unsigned int
    keyHeight() const;

    void
    setKeyHeight(unsigned int keyHeight);

    void
    setSymbolState(unsigned char index);

    void
    setKeyFont(Font* font);

protected:
    void
    compose(const PaintEvent& event);

private:
    std::string _xmlID;
    unsigned int _gap;
    unsigned int _keyHeight;
    std::vector<Key*> _keys;
    HBoxLayout* _box;

    void
    updateRowGeometry();
};

} /* namespace ilixi */
#endif /* ROW_H_ */
