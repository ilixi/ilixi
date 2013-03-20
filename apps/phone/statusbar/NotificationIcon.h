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

#ifndef NOTIFICATIONICON_H_
#define NOTIFICATIONICON_H_

#include <ui/Widget.h>
#include <types/Image.h>
#include <vector>

namespace ilixi
{

class NotificationIcon : public Widget
{
public:
    NotificationIcon(Widget* parent = 0);

    virtual
    ~NotificationIcon();

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
