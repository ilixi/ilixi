/*
 * Key.cpp
 *
 *  Created on: Aug 6, 2012
 *      Author: tarik
 */

#include "Key.h"
#include "utf8-decoder.h"

namespace ilixi
{

Key::Key(const std::string& text, Widget* parent)
        : ToolButton(text, parent),
          _keyMode(Default),
          _state(0)
{
    setInputMethod(PointerInput);
    setConstraints(MinimumConstraint, MinimumConstraint);
    setToolButtonStyle(TextOnly);

    sigClicked.connect(sigc::mem_fun(this, &Key::pressSlot));
}

Key::~Key()
{
}

unsigned char
Key::symbolState() const
{
    return _state;
}

void
Key::setSymbolState(unsigned char state)
{
    _state = state;
    setText(_symbols[_state]);
}

Key::KeyMode
Key::keyMode() const
{
    return _keyMode;
}

void
Key::setKeyMode(KeyMode keyMode)
{
    _keyMode = keyMode;
}

void
Key::addSymbol(unsigned int state, const std::string& symbol)
{
    decode((uint8_t*) symbol.c_str());
    _symbols.insert(std::pair<unsigned int, std::string>(state, symbol));
}

void
Key::pressSlot()
{
    // encode symbols
    // _target->consumeKeyEvent(KeyEvent(KeyDownEvent, _symbols[_index]));
}

} /* namespace ilixi */
