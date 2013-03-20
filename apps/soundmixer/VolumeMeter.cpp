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

#include "VolumeMeter.h"
#include <graphics/Painter.h>
#include <core/SoundDFB.h>

namespace ilixi
{

VolumeMeter::VolumeMeter(Widget* parent)
        : Widget(parent),
          _chanLeft(0),
          _chanRight(0)
{
    setConstraints(FixedConstraint, FixedConstraint);
    _volumeOff = new Image(ILIXI_DATADIR"soundmixer/meter_off.png");
    _volumeOn = new Image(ILIXI_DATADIR"soundmixer/meter_on.png");
    _volTimer.sigExec.connect(sigc::mem_fun(this, &VolumeMeter::updateVolume));
    _volTimer.start(50);
}

VolumeMeter::~VolumeMeter()
{
}

Size
VolumeMeter::preferredSize() const
{
    return Size(64, 304);
}

void
VolumeMeter::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    p.drawImage(_volumeOff, 0, 0);
    p.drawImage(_volumeOff, 32, 0);
    drawMeterValue(&p, 0, _chanLeft);
    drawMeterValue(&p, 32, _chanRight);
    p.end();
}

void
VolumeMeter::drawMeterValue(Painter* p, int x, float value)
{
    if (value < 0.0001f)
        return;

    int split = 300 * value * 0.5f;

    split = (split + 11) / 12 * 12;

    p->blitImage(_volumeOn, Rectangle(0, 304 - 3 - split, 32, 3 + split), x,
                 304 - 3 - split);
}

void
VolumeMeter::updateVolume()
{
    float left, right;
    SoundDFB::getMasterAmplitude(&left, &right);
    if (left != _chanLeft || right != _chanRight)
    {
        _chanLeft = left;
        _chanRight = right;
        update();
    }

}

} /* namespace ilixi */
