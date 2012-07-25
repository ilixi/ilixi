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
    IComaComponent* _soundComponent;
    Slider* _volSlider;
    ToolButton* _mute;

    void
    changeVolume(int volume);
};

} /* namespace ilixi */
#endif /* SOUNDMIXER_H_ */
