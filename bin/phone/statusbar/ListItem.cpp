/*
 * MenuItem.cpp
 *
 *  Created on: Dec 5, 2012
 *      Author: tarik
 */

#include "ListItem.h"
#include <graphics/Painter.h>

namespace ilixi {

ListItem::ListItem(const std::string& title, Widget* parent) :
		ToolButton(title, parent) {
	setDrawFrame(false);
	_layout.setSingleLine(false);
	_layout.setAlignment(TextLayout::Left);
}

ListItem::~ListItem() {
}

void ListItem::setBg(Image* bg) {
	_bg = bg;
}

Size ListItem::preferredSize() const {
	return Size(120, 51);
}

void ListItem::compose(const PaintEvent& event) {
	Painter p(this);
	p.begin(event);
	if (state() & FocusedState)
		p.drawImage(_bg, 0, 0);

	p.setFont(*font());
	p.setBrush(Color(255, 255, 255));
	p.drawLayout(_layout, 0, state() & PressedState ? 1 : 0);
}

}
