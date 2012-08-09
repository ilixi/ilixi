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
#include <vector>

namespace ilixi
{

class Keyboard;

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

    Key(const std::string& id, Keyboard* keyboard, Widget* parent = 0);

    virtual
    ~Key();

    std::string
    xmlID() const;

    unsigned char
    symbolState() const;

    void
    setSymbolState(unsigned char state);

    KeyMode
    keyMode() const;

    void
    setKeyMode(KeyMode keyMode);

    void
    addSymbol(const std::string& states, const std::string& symbol);

    void
    setRollStates(const std::string& rollStates);

private:
    std::string _xmlID;
    KeyMode _keyMode;
    unsigned char _state;
    Keyboard* _keyboard;
    typedef std::list<unsigned char> RollStateList;
    RollStateList _rollStates;

    struct utf8Data
    {
        std::string str;
        std::vector<uint32_t> ucs32;
    };

    typedef std::map<unsigned char, utf8Data> SymbolMap;
    SymbolMap _symbols;

    void
    pressSlot();
};

} /* namespace ilixi */
#endif /* KEY_H_ */
