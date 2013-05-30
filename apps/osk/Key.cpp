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

#include "Key.h"
#include "utf8-decoder.h"
#include "Keyboard.h"
#include <ui/HBoxLayout.h>
#include <ui/Label.h>
#include <graphics/Painter.h>
#include <core/Logger.h>
#include <algorithm>

#include <direct/util.h>
#include <directfb_keynames.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_KEY, "ilixi/osk/Key", "Key");

static const DirectFBKeySymbolNames( symbol_names );

Key::Key(const std::string& id, Keyboard* keyboard, Widget* parent)
        : ToolButton(id, parent),
          _xmlID(id),
          _keyMode(Default),
          _keyState(1),
          _keyboard(keyboard)
{
    setConstraints(MinimumConstraint, NoConstraint);
    setToolButtonStyle(TextOnly);

    sigClicked.connect(sigc::mem_fun(this, &Key::pressSlot));
}

Key::~Key()
{
}

std::string
Key::xmlID() const
{
    return _xmlID;
}

unsigned char
Key::symbolState() const
{
    return _keyState;
}

void
Key::setSymbolState(unsigned char state)
{
    if (!(_keyMode & Modifier) && (state > _symbols.size()))
        setDisabled();
    else if (_keyMode & Modifier)
    {
        RollStateList::iterator it = std::find(_rollStates.begin(), _rollStates.end(), state);
        if (state == *it)
        {
            _keyState = state;
            setEnabled();
            if (_keyState == _rollStates[1])
                setChecked(true);
            else
                setChecked(false);
        } else
        {
            setDisabled();
            setChecked(false);
        }
    } else
    {
        _keyState = state;

        setText(_symbols[_keyState].str);

        ILOG_DEBUG( ILX_KEY, "%s (%d, %s)\n", _xmlID.c_str(), _keyState, _symbols[_keyState].str.c_str());
    }
}

Key::KeyMode
Key::keyMode() const
{
    return _keyMode;
}

void
Key::setKeyMode(KeyMode keyMode)
{
    _keyMode = (KeyMode) (_keyMode | keyMode);

    if (_keyMode & Modifier)
        setCheckable(true);
}

void
Key::addSymbol(const std::string& states, const std::string& symbol)
{
    char* pch = strtok(const_cast<char*>(states.c_str()), " ,");
    while (pch != NULL)
    {
        unsigned char state = atoi(pch);
        utf8Data keyData;
        ILOG_DEBUG(ILX_KEY, "State: %d\n", state);

        std::string sym;

        if (_keyMode & Cycle)
        {
            _cycleLabel = symbol.substr(0, 1);

            std::vector<uint32_t> ucs32;
            decode((uint8_t*) _cycleLabel.c_str(), ucs32);
            _cycleUCS = ucs32[0];

            sym = symbol.substr(1);
        } else
            sym = symbol;

        if (_keyMode & Special)
        {
            for (int i = 0; i < D_ARRAY_SIZE(symbol_names) ; i++)
                if (symbol_names[i].name == sym)
                    keyData.ucs32.push_back(symbol_names[i].symbol);
        } else
            decode((uint8_t*) sym.c_str(), keyData.ucs32);
        keyData.str = sym;

        _symbols.insert(std::pair<unsigned char, utf8Data>(state, keyData));

        pch = strtok(NULL, " ,");
    }
}

void
Key::setRollStates(const std::string& rollStates)
{
    _rollStates.clear();
    char* pch = strtok(const_cast<char*>(rollStates.c_str()), " ,");
    while (pch != NULL)
    {
        _rollStates.push_back(atoi(pch));
        if ((_keyMode & Modifier) && _rollStates.size() == 2)
            break;
        pch = strtok(NULL, " ,");
    }
}

void
Key::toggleChecked()
{
    update();
}

void
Key::compose(const PaintEvent& event)
{
    ToolButton::compose(event);

    if (!_cycleLabel.empty())
    {
        Painter p(this);
        p.begin(event);
        p.setBrush(stylist()->palette()->getGroup(state()).text);
        p.drawText(_cycleLabel, stylist()->defaultParameter(StyleHint::PanelLeft), stylist()->defaultParameter(StyleHint::PanelTop));
    }
}

void
Key::pressSlot()
{
    if (_keyMode & Helper)
        _keyboard->toggleHelper();
    else if (_keyMode & Modifier)
    {
        ILOG_DEBUG(ILX_KEY, "Modifier state: %d\n", _keyState);
        if (_keyState == _rollStates[0])
            _keyboard->setModifier(this);
        _keyboard->setSymbolState(getNextState());
    } else if (_keyMode & Cycle)
    {
        ILOG_DEBUG(ILX_KEY, "Cycle key\n");

        _keyboard->handleCycleKey(this);
    } else if (_keyMode & Sticky)
    {
        ILOG_DEBUG(ILX_KEY, "Sticky state: %d\n", _keyState);
        if (_keyState == _symbols.size())
            _keyboard->setSymbolState(1);
        else
            _keyboard->setSymbolState(_keyState + 1);
    } else
        _keyboard->forwardKeyData(_symbols[_keyState].ucs32);
}

unsigned char
Key::getNextState()
{
    unsigned int next = _rollStates[0];
    for (unsigned int i = 0; i < _rollStates.size(); ++i)
    {
        if (_keyState == _rollStates[i])
        {
            if (i + 1 < _rollStates.size())
                next = _rollStates[i + 1];
            break;
        }
    }
    return next;
}

} /* namespace ilixi */
