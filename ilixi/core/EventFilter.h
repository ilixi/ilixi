/*
 Copyright 2010-2015 Tarik Sekmen.

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

#ifndef ILIXI_EVENTFILTER_H_
#define ILIXI_EVENTFILTER_H_

#include <types/Event.h>

namespace ilixi
{

//! This class is used to intercept and process incoming key and pointer input events in widgets.
class EventFilter
{
public:
    EventFilter();

    virtual
    ~EventFilter();

    /*!
     * Intercepts incoming key events.
     *
     * Event will not be processed further by a widget if this method returns True.
     *
     * @return true if event is consumed, false otherwise.
     */
    virtual bool
    keyEventConsumer(const KeyEvent& keyEvent);

    /*!
     * Intercepts incoming pointer events.
     *
     * Event will not be processed further by a widget if this method returns True.
     *
     * @return true if event is consumed, false otherwise.
     */
    virtual bool
    pointerEventConsumer(const PointerEvent& pointerEvent);
};

} /* namespace ilixi */
#endif /* ILIXI_EVENTFILTER_H_ */
