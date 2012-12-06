/*
 * NotificationIcon.cpp
 *
 *  Created on: Dec 5, 2012
 *      Author: tarik
 */

#include "NotificationIcon.h"
#include <graphics/Painter.h>

namespace ilixi {

NotificationIcon::NotificationIcon(Widget* parent) :
		Widget(parent), _state(-1) {

}

NotificationIcon::~NotificationIcon() {
	for (StateVector::iterator it = _states.begin(); it != _states.end(); ++it)
		delete *it;
}

Size NotificationIcon::preferredSize() const {
	return Size(32, 32);
}

void NotificationIcon::addState(const std::string& icon) {
	_state = _states.size();
	_states.push_back(new Image(icon));
}

unsigned int NotificationIcon::state() const {
	return _state;
}

void NotificationIcon::setState(unsigned int state) {
	if (state < _states.size())
		_state = state;
}

void NotificationIcon::compose(const PaintEvent& event) {
	if (_state == -1)
		return;

	Painter p(this);
	p.begin(event);
	p.drawImage(_states[_state], 0, 0);
}

} /* namespace ilixi */
