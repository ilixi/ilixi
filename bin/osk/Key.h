/*
 * Key.h
 *
 *  Created on: Aug 6, 2012
 *      Author: tarik
 */

#ifndef KEY_H_
#define KEY_H_

#include <ui/ToolButton.h>
#include <map>

namespace ilixi
{

class Key : public ToolButton
{
public:
    enum KeyMode
    {
        Default = 0x001, //!< Default key behaviour.
        Modifier = 0x002, //!< Checkable modifier key, e.g. SHIFT on/off.
        Sticky = 0x004, //!< Toggle key.
        Repeatable = 0x008 //!< Long-press will repeat.
    };

    Key(const std::string& id, Widget* parent = 0);

    virtual
    ~Key();

    unsigned char
    symbolState() const;

    void
    setSymbolState(unsigned char state);

    KeyMode
    keyMode() const;

    void
    setKeyMode(KeyMode keyMode);

    void
    addSymbol(unsigned int state, const std::string& symbol);

private:
    KeyMode _keyMode;
    unsigned char _state;

    struct utf8Data
    {
        std::string str;
        uint32_t* data;
    };

    std::map<unsigned int, std::string> _symbols;

    void
    pressSlot();
};

} /* namespace ilixi */
#endif /* KEY_H_ */
