/*
 Copyright 2010-2012 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>,
            Denis Oliver Kropp <dok@directfb.org>.

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

#ifndef VOLUMEMETER_H_
#define VOLUMEMETER_H_

#include <ui/Widget.h>
#include <lib/Timer.h>

namespace ilixi
{

class VolumeMeter : public Widget
{
public:
    VolumeMeter(Widget* parent = 0);

    virtual
    ~VolumeMeter();

    Size
    preferredSize() const;

protected:
    virtual void
    compose(const PaintEvent& event);

private:
    float _chanLeft;
    float _chanRight;
    Image* _volumeOff;
    Image* _volumeOn;
    Timer _volTimer;

    void
    drawMeterValue(Painter* painter, int x, float value);

    void
    updateVolume();
};

} /* namespace ilixi */
#endif /* VOLUMEMETER_H_ */
