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

#include "SoundMixer.h"
#include <ui/VBoxLayout.h>
#include <ui/Spacer.h>
#include <core/Logger.h>
#include <lib/Notify.h>
#include "VolumeMeter.h"

namespace ilixi
{

SoundMixer::SoundMixer(int argc, char* argv[])
        : Application(&argc, &argv, OptDale)
{
    setTitle("SoundMixer");
    setLayout(new VBoxLayout());

    addWidget(new Spacer(Vertical));

    VolumeMeter* meter = new VolumeMeter();
    addWidget(meter);

    _volSlider = new Slider();
    addWidget(_volSlider);

    _mute = new ToolButton("Mute");
    addWidget(_mute);
    _mute->sigClicked.connect(sigc::mem_fun(this, &SoundMixer::mute));

    addWidget(new Spacer(Vertical));

    _volSlider->sigValueChanged.connect(
            sigc::mem_fun(this, &SoundMixer::changeVolume));

    AppBase::comaGetComponent("SoundComponent", &_soundComponent);
}

SoundMixer::~SoundMixer()
{
    _soundComponent->Release(_soundComponent);
}

void
SoundMixer::mute()
{
    Notify notify(
            "Parmak!",
            "Yusuf likes big parmak... Yusuf likes big parmak... Yusuf likes big parmak... Yusuf likes big parmak...");
    notify.setIcon(ILIXI_DATADIR"images/default.png");
    notify.show();
}

void
SoundMixer::changeVolume(int volume)
{
    void *ptr;
    int result;
    comaGetLocal(sizeof(int), &ptr);
    int* vol = (int*) ptr;
    *vol = volume;

    AppBase::comaCallComponent(_soundComponent, 0, (void*) vol);
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    ilixi::SoundMixer app(argc, argv);
    app.exec();
    return 0;
}
