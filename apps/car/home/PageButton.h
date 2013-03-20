/*
 * PageButton.h
 *
 *  Created on: Sep 4, 2012
 *      Author: tarik
 */

#ifndef PAGEBUTTON_H_
#define PAGEBUTTON_H_

#include <ui/ToolButton.h>

namespace ilixi
{

class PageButton : public ToolButton
{
public:
    PageButton(const std::string& text, Widget* parent = 0);

    virtual
    ~PageButton();

protected:
    void
    compose(const PaintEvent& event);
};

} /* namespace ilixi */
#endif /* PAGEBUTTON_H_ */
