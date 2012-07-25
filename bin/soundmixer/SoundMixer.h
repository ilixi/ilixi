/*
 * SoundMixer.h
 *
 *  Created on: Jul 23, 2012
 *      Author: tarik
 */

#ifndef SOUNDMIXER_H_
#define SOUNDMIXER_H_

#include <ui/Application.h>
#include <ui/Slider.h>
#include <ui/ToolButton.h>
#include "SoundComponent.h"

namespace ilixi
{

class SoundMixer : public Application
{
public:
    SoundMixer(int argc, char* argv[]);

    virtual
    ~SoundMixer();

    void
    mute();

private:
    SoundComponent* _comp;
    Slider* _volSlider;
    ToolButton* _mute;
};

} /* namespace ilixi */
#endif /* SOUNDMIXER_H_ */
