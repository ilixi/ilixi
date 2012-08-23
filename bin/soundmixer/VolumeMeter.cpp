/*
 * VolumeMeter.cpp
 *
 *  Created on: Aug 23, 2012
 *      Author: tarik
 */

#include "VolumeMeter.h"
#include <graphics/Painter.h>
#include <core/Logger.h>

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

    p->blitImage(_volumeOn, Rectangle(0, 304 - 3 - split, 32, 3 + split), x, 304 - 3 - split);

/*
    if (value < 0.08)
        p->blitImage(_volumeOn, Rectangle(0, 289, 32, 15), x, 289);
    else if (value < 0.16)
        p->blitImage(_volumeOn, Rectangle(0, 277, 32, 27), x, 277);
    else if (value < 0.24)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 0.32)
        p->blitImage(_volumeOn, Rectangle(0, 253, 32, 51), x, 253);
    else if (value < 0.40)
        p->blitImage(_volumeOn, Rectangle(0, 241, 32, 63), x, 241);
    else if (value < 0.48)
        p->blitImage(_volumeOn, Rectangle(0, 229, 32, 75), x, 229);
    else if (value < 0.56)
        p->blitImage(_volumeOn, Rectangle(0, 217, 32, 87), x, 217);
    else if (value < 0.64)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 0.72)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 0.80)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 0.88)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 0.96)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 1.04)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 1.12)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 1.20)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 1.28)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 1.36)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 1.44)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 1.52)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 1.60)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 1.68)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 1.76)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 1.84)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else if (value < 1.82)
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
    else
        p->blitImage(_volumeOn, Rectangle(0, 265, 32, 39), x, 265);
*/
}

void
VolumeMeter::updateVolume()
{
    _sound.getMasterAmplitude(&_chanLeft, &_chanRight);
    ILOG_INFO(ILX, "%f - %f\n", _chanLeft, _chanRight);
    update();
}

} /* namespace ilixi */
