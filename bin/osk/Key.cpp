/*
 * Key.cpp
 *
 *  Created on: Aug 6, 2012
 *      Author: tarik
 */

#include "Key.h"
#include "utf8-decoder.h"
#include "Keyboard.h"
#include <core/Logger.h>
#include <algorithm>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_KEY, "ilixi/osk/Key", "Key");

Key::Key(const std::string& id, Keyboard* keyboard, Widget* parent)
        : ToolButton(id, parent),
          _xmlID(id),
          _keyMode(Default),
          _state(1),
          _keyboard(keyboard)
{
    setInputMethod(PointerInput);
    setConstraints(MinimumConstraint, MinimumConstraint);
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
    return _state;
}

void
Key::setSymbolState(unsigned char state)
{
    if (!(_keyMode & Modifier) && (state > _symbols.size()))
        setDisabled();
    else if (_keyMode & Modifier)
    {
        RollStateList::iterator it = std::find(_rollStates.begin(), _rollStates.end(),
                                     state);
        if (state == *it)
            _state = state;
        else
            setDisabled();
    } else
    {
        _state = state;
        setText(_symbols[_state].str);
        ILOG_DEBUG(ILX_KEY,
                   "State %d Text %s\n", _state, _symbols[_state].str.c_str());
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
        decode((uint8_t*) symbol.c_str(), keyData.ucs32);
        keyData.str = symbol;

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
        pch = strtok(NULL, " ,");
    }
}

void
Key::pressSlot()
{
    if (_keyMode & Modifier)
    {
        ILOG_DEBUG(ILX_KEY, "Modifier state: %d\n", _state);
        _state + 1;
        _keyboard->setSymbolState(_state);
    } else if (_keyMode & Sticky)
    {
        ILOG_DEBUG(ILX_KEY, "Sticky state: %d\n", _state);
        if (_state == _symbols.size())
            _state = 0;
        _keyboard->setSymbolState(_state + 1);
    } else
    {
        // encode symbols
        // _target->consumeKeyEvent(KeyEvent(KeyDownEvent, _symbols[_index]));
    }
}

} /* namespace ilixi */
