/*
 * NotificationIcon.h
 *
 *  Created on: Dec 5, 2012
 *      Author: tarik
 */

#ifndef NOTIFICATIONICON_H_
#define NOTIFICATIONICON_H_

#include <ui/Widget.h>
#include <types/Image.h>
#include <vector>

namespace ilixi {

class NotificationIcon: public Widget {
public:
	NotificationIcon(Widget* parent = 0);

	virtual ~NotificationIcon();

	Size
	preferredSize() const;

	void
	addState(const std::string& icon);

	unsigned int
	state() const;

	void
	setState(unsigned int state);

protected:
	void
	compose(const PaintEvent& event);

private:
	unsigned int _state;
	typedef std::vector<Image*> StateVector;
	StateVector _states;
};

} /* namespace ilixi */
#endif /* NOTIFICATIONICON_H_ */
