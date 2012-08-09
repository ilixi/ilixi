/*
 * Keyboard.h
 *
 *  Created on: Aug 6, 2012
 *      Author: tarik
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "Row.h"
#include <libxml/tree.h>

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
};

} /* namespace ilixi */
#endif /* KEYBOARD_H_ */
