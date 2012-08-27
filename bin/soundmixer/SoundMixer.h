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

#include <ui/Application.h>
#include <core/DaleDFB.h>

namespace ilixi
{
class Slider;
class PushButton;

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
    PushButton* _mute;

    void
    changeVolume(int volume);
};

} /* namespace ilixi */
#endif /* SOUNDMIXER_H_ */
