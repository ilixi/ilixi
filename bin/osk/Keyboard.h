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
#include <types/Font.h>
#include <lib/Timer.h>
#include <libxml/tree.h>
#include <core/DaleDFB.h>
#include "Helper.h"

namespace ilixi
{

class Keyboard : public Widget
{
public:
    Keyboard(OSKHelper* helper, Widget* parent = 0);

    virtual
    ~Keyboard();

    void
    toggleHelper();

    void
    setSymbolState(unsigned char state);

    bool
    parseLayoutFile(const char* file);

    void
    forwardKeyData(const uint32_t& ucs32, unsigned int modifiers = 0);

    void
    forwardKeyData(const std::vector<uint32_t>& ucs32, unsigned int modifiers = 0, bool force = false);

    void
    setModifier(Key* modifier);

    void
    handleCycleKey(Key* key);

    bool
    handleKeyPress(uint32_t symbol);

protected:
    void
    compose(const PaintEvent& event);

private:
    //! Visualisation for InputHelper (only JP atm).
    OSKHelper* _helper;
    //! This font is shared by all keys.
    Font* _buttonFont;
    //! Compositor's osk component.
    IComaComponent* _oskComponent;
    //! This stores rows for ease of access.
    std::vector<Row*> _rows;
    //! Current modifier key.
    Key* _modifier;
    //! Current cycling key.
    Key* _cycleKey;

    uint32_t _cycleCharacter;
    Timer _cycleTimer;

    std::map<uint32_t, Key*> _cycleMap;

    Key*
    getKey(xmlNodePtr node);

    Row*
    getRow(xmlNodePtr node);

    void
    release();

    void
    updateKeyboardGeometry();

    void
    handleCycleTimer();
};

} /* namespace ilixi */
#endif /* ILIXI_KEYBOARD_H_ */
