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

#ifndef ILIXI_KEYBOARD_H_
#define ILIXI_KEYBOARD_H_

#include "Row.h"
#include <libxml/tree.h>
extern "C"
{
#include <fusiondale.h>
}

namespace ilixi
{

class Keyboard : public Widget
{
public:
    Keyboard(Widget* parent = 0);

    virtual
    ~Keyboard();

    void
    setSymbolState(unsigned char state);

    bool
    parseLayoutFile(const char* file);

    void
    forwardKeyData(const std::vector<uint32_t>& ucs32);

protected:
    void
    compose(const PaintEvent& event);

private:
    std::vector<Row*> _rows;

    Key*
    getKey(xmlNodePtr node);

    Row*
    getRow(xmlNodePtr node);

    void
    release();

    void
    updateKeyboardGeometry();

    //! Compositor's osk component.
    IComaComponent* _oskComponent;
};

} /* namespace ilixi */
#endif /* ILIXI_KEYBOARD_H_ */
