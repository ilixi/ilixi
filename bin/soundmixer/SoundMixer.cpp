/*
 * SoundMixer.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: tarik
 */

#include "SoundMixer.h"
#include <ui/VBoxLayout.h>
#include <ui/Spacer.h>
#include <core/Logger.h>
#include <lib/Notify.h>

namespace ilixi
{

SoundMixer::SoundMixer(int argc, char* argv[])
        : Application(&argc, &argv)
{
    setTitle("SoundMixer");
    setLayout(new VBoxLayout());

    addWidget(new Spacer(Vertical));

    _volSlider = new Slider();
    addWidget(_volSlider);

    _mute = new ToolButton("Mute");
    addWidget(_mute);
    _mute->sigClicked.connect(sigc::mem_fun(this, &SoundMixer::mute));

    addWidget(new Spacer(Vertical));
//
//    _volSlider->sigValueChanged.connect(
//            sigc::mem_fun(_comp, &SoundComponent::setVolume));
}

SoundMixer::~SoundMixer()
{
}

void
SoundMixer::mute()
{
    Notify notify("Parmak!",
            "Yusuf likes big parmak... Yusuf likes big parmak... Yusuf likes big parmak... Yusuf likes big parmak...");
    notify.setIcon(ILIXI_DATADIR"images/default.png");
    notify.show();
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    ilixi::SoundMixer app(argc, argv);
    app.exec();
    return 0;
}
