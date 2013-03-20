/*
 * PageButton.cpp
 *
 *  Created on: Sep 4, 2012
 *      Author: tarik
 */

#include "PageButton.h"
#include <graphics/Painter.h>
#include "Home.h"

namespace ilixi
{

PageButton::PageButton(const std::string& text, Widget* parent)
        : ToolButton(text, parent)
{

}

PageButton::~PageButton()
{
}

void
PageButton::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    p.setBrush(Color(255, 255, 255));
    if (state() & DisabledState)
        p.drawImage(Home::_circle, 0, 0);
    else
        p.drawImage(Home::_circle_sm, 5, 5);
    p.setFont(*font());
    p.drawLayout(_layout);
    p.end();
}

} /* namespace ilixi */
