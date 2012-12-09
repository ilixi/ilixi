/*
 * MenuItem.h
 *
 *  Created on: Dec 5, 2012
 *      Author: tarik
 */

#ifndef LISTITEM_H_
#define LISTITEM_H_

#include <ui/ToolButton.h>

namespace ilixi
{

class ListItem : public ToolButton
{
public:
    ListItem(const std::string& title, Widget* parent = 0);

    virtual
    ~ListItem();

    Size
    preferredSize() const;

    void
    setBg(Image* bg);

protected:
    void
    compose(const PaintEvent& event);

    virtual void
    focusInEvent();

    virtual void
    focusOutEvent();

private:
    Image* _bg;
};

}
#endif /* LISTITEM_H_ */
