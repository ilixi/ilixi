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
public:
    Row(const std::string& id, Widget* parent = 0);

    virtual
    ~Row();

    std::string
    id() const;

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
    setRowSymbolIndex(unsigned char index);

private:
    std::string _id;
    unsigned int _gap;
    unsigned int _keyHeight;
    std::vector<Key*> _keys;
    HBoxLayout* _box;

    void
    updateRowGeometry();
};

} /* namespace ilixi */
#endif /* ROW_H_ */
