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

#ifndef CLOCK_H_
#define CLOCK_H_

#include <ui/Label.h>
#include <lib/Timer.h>
#include <ui/VBoxLayout.h>

namespace ilixi
{

class Clock : public Widget
{
public:
    Clock(Widget* parent = 0);

    virtual
    ~Clock();

    virtual Size
    preferredSize() const;

protected:
    virtual void
    compose(const PaintEvent& event);

    void
    updateTime();

private:
    Label* _time;
    Label* _date;
    Timer* _timer;
    VBoxLayout* _box;

    void
    onClockGeomUpdate();
};

} /* namespace ilixi */
#endif /* CLOCK_H_ */
