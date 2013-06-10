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

#ifndef SOUNDMIXER_H_
#define SOUNDMIXER_H_

#include <core/Application.h>
#include <core/DaleDFB.h>

namespace ilixi
{
class Slider;
class PushButton;
class Music;

class ILXSoundMixer : public Application
{
public:
    ILXSoundMixer(int argc, char* argv[]);

    virtual
    ~ILXSoundMixer();

    void
    mute();

    void
    playTestSound();

private:
    IComaComponent* _soundComponent;
    Music* _music;
    Font* _bandSliderFont;
    Slider* _masterVolume;
    Slider* _effectsVolume;
    PushButton* _mute;

    void
    changeMasterVolume(float volume);

    void
    changeEffectsVolume(float volume);

    friend void
    volumeListener(void* ctx, void* arg);
};

} /* namespace ilixi */
#endif /* SOUNDMIXER_H_ */
