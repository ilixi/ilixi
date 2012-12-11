/*
 * Row.h
 *
 *  Created on: Aug 6, 2012
 *      Author: tarik
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
