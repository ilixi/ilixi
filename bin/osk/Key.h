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

#ifndef ILIXI_KEY_H_
#define ILIXI_KEY_H_

#include <ui/ToolButton.h>
#include <map>
#include <vector>

namespace ilixi
{

class Keyboard;

/*!
 * This class provides a generic keyboard key.
 *
 * Key behavior is controlled by KeyMode.
 */
class Key : public ToolButton
{
    friend class Keyboard;
public:
    enum KeyMode
    {
        Default = 0x001,    //!< Default key behavior.
        Modifier = 0x002,   //!< Checkable modifier key will return back to previous state after key press, e.g. SHIFT on/off. A modifier can only have two states.
        Sticky = 0x004,     //!< Toggles key states.
        Special = 0x008,    //!< Key can symbol is set to a DIKS_XX.
        Cycle = 0x010,      //!< Cycle through characters, e.g. for phone input.
        Helper = 0x020      //!< Helper key.
    };

    /*!
     * Constructor
     * @param id xmlID
     * @param keyboard owner.
     * @param parent
     */
    Key(const std::string& id, Keyboard* keyboard, Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~Key();

    /*!
     * Returns xmlID of this key.
     */
    std::string
    xmlID() const;

    /*!
     * Returns current state of this key.
     */
    unsigned char
    symbolState() const;

    /*!
     * Sets state of this key.
     */
    void
    setSymbolState(unsigned char state);

    /*!
     * Returns key operation mode of this key.
     */
    KeyMode
    keyMode() const;

    /*!
     * Sets given key mode flag to ON.
     */
    void
    setKeyMode(KeyMode keyMode);

    /*!
     * Adds new state symbol pairs to map.
     * @param states a list of comma separated states.
     * @param symbol key symbols for given states.
     */
    void
    addSymbol(const std::string& states, const std::string& symbol);

    /*!
     * Sets the states which this key can switch between when in modifier mode.
     */
    void
    setRollStates(const std::string& rollStates);

    virtual void
    toggleChecked();

    virtual void
    compose(const PaintEvent& event);

private:
    //! This property stores the xmlID of key.
    std::string _xmlID;
    //! This property specifies key operation modes.
    KeyMode _keyMode;
    //! This property specifies current key state.
    unsigned char _keyState;
    //! This is owner.
    Keyboard* _keyboard;
    //! extra label for cycle key
    std::string _cycleLabel;
    //! This is added to cycle map in keyboard.
    uint32_t _cycleUCS;

    typedef std::vector<unsigned char> RollStateList;
    //! List of states which this key can set.
    RollStateList _rollStates;

    struct utf8Data
    {
        std::string str;
        std::vector<uint32_t> ucs32;
    };

    typedef std::map<unsigned char, utf8Data> SymbolMap;
    //! This property stores the symbol map of this key.
    SymbolMap _symbols;

    void
    pressSlot();

    unsigned char
    getNextState();
};

} /* namespace ilixi */
#endif /* ILIXI_KEY_H_ */
