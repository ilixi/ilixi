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

#ifndef ILIXI_SOUNDCOMPONENT_H_
#define ILIXI_SOUNDCOMPONENT_H_

#include <core/ComaComponent.h>

namespace ilixi
{

class SoundComponent : public ComaComponent
{
public:
    SoundComponent();

    virtual
    ~SoundComponent();

    void
    setVolume(float volume);

protected:
    virtual DirectResult
    comaMethod(ComaMethodID method, void *arg);

private:
    float _volume;
};

} /* namespace ilixi */
#endif /* ILIXI_SOUNDCOMPONENT_H_ */
