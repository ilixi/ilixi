/*
 * VolumeMeter.h
 *
 *  Created on: Aug 23, 2012
 *      Author: tarik
 */

#ifndef VOLUMEMETER_H_
#define VOLUMEMETER_H_

#include <ui/Widget.h>
#include <lib/Timer.h>
#include <lib/Sound.h>

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
    Sound _sound;

    void
    drawMeterValue(Painter* painter, int x, float value);

    void
    updateVolume();
};

} /* namespace ilixi */
#endif /* VOLUMEMETER_H_ */
