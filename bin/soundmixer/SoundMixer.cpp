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
#include <ui/GroupBox.h>
#include <ui/VBoxLayout.h>
#include <ui/HBoxLayout.h>
#include <ui/GridLayout.h>
#include <ui/PushButton.h>
#include <ui/CheckBox.h>
#include <ui/Spacer.h>
#include <core/Logger.h>
#include <lib/Notify.h>
#include "VolumeMeter.h"
#include "BandSlider.h"

#include <types/Music.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_MIXERAPP, "ilixi/Mixer", "SoundMixer");

SoundMixer::SoundMixer(int argc, char* argv[])
        : Application(&argc, &argv, AppOptions(OptDale | OptSound)),
          _soundComponent(NULL),
          _music(NULL)
{
    setTitle("SoundMixer");
    setBackgroundImage(ILIXI_DATADIR"compositor/bg.png");
    setMargin(20);
    setLayout(new VBoxLayout());

    HBoxLayout* volStuff = new HBoxLayout();
    addWidget(volStuff);

    //**********
    GroupBox* volGroup = new GroupBox("Volume Control");
    GridLayout* volBox = new GridLayout(6, 3);
    volGroup->setLayout(volBox);
    volStuff->addWidget(volGroup);

    volBox->addWidget(new Label("Output:"), 0, 0, 0, 3);

    _volSlider = new Slider();
    _volSlider->setValue(100);
    _volSlider->sigValueChanged.connect(
            sigc::mem_fun(this, &SoundMixer::changeVolume));
    volBox->addWidget(_volSlider, 1, 0, 0, 2);

    _mute = new PushButton("Mute");
    _mute->setXConstraint(FixedConstraint);
    volBox->addWidget(_mute, 1, 2);
    _mute->sigClicked.connect(sigc::mem_fun(this, &SoundMixer::mute));

    volBox->addWidget(new Label("Balance:"), 2, 0, 0, 3);
    volBox->addWidget(new Label("Front"), 3, 0);
    Slider* frSlider = new Slider();
    frSlider->setValue(50);
    volBox->addWidget(frSlider, 3, 1);
    volBox->addWidget(new Label("Rear"), 3, 2);
    volBox->addWidget(new Label("Left"), 4, 0);
    Slider* lrSlider = new Slider();
    lrSlider->setValue(50);
    volBox->addWidget(lrSlider, 4, 1);
    volBox->addWidget(new Label("Right"), 4, 2);
    volBox->addWidget(new Spacer(Vertical), 5, 0);

    GroupBox* volMeter = new GroupBox("Meter");
    VBoxLayout* volMeterLayout = new VBoxLayout();
    volMeterLayout->setHorizontalAlignment(Alignment::Center);
    volMeter->setLayout(volMeterLayout);
    volMeter->setXConstraint(FixedConstraint);
    volStuff->addWidget(volMeter);
    VolumeMeter* meter = new VolumeMeter();
    volMeter->addWidget(meter);

    //*********
    GroupBox* levels = new GroupBox("Equalizer");
    VBoxLayout* eqBox = new VBoxLayout();
    levels->setLayout(eqBox);
    addWidget(levels);

    HBoxLayout* buttons = new HBoxLayout();
    buttons->addWidget(new CheckBox("EQ Enabled"));
    buttons->addWidget(new Spacer(Horizontal));
    buttons->addWidget(new PushButton("Load Preset"));
    buttons->addWidget(new PushButton("Save Preset"));
    PushButton* testSound = new PushButton("Test");
    testSound->sigClicked.connect(
            sigc::mem_fun(this, &SoundMixer::playTestSound));
    buttons->addWidget(testSound);
    levels->addWidget(buttons);

    HBoxLayout* rowLevels = new HBoxLayout();
    rowLevels->setYConstraint(ExpandingConstraint);
    rowLevels->addWidget(new BandSlider("50Hz"));
    rowLevels->addWidget(new BandSlider("100Hz"));
    rowLevels->addWidget(new BandSlider("156Hz"));
    rowLevels->addWidget(new BandSlider("220Hz"));
    rowLevels->addWidget(new BandSlider("311Hz"));
    rowLevels->addWidget(new BandSlider("440Hz"));
    rowLevels->addWidget(new BandSlider("622Hz"));
    rowLevels->addWidget(new BandSlider("880Hz"));
    rowLevels->addWidget(new BandSlider("1.25KHz"));
    rowLevels->addWidget(new BandSlider("1.75KHz"));
    rowLevels->addWidget(new BandSlider("2.5KHz"));
    rowLevels->addWidget(new BandSlider("3.5KHz"));
    rowLevels->addWidget(new BandSlider("5KHz"));
    rowLevels->addWidget(new BandSlider("10KHz"));
    rowLevels->addWidget(new BandSlider("20KHz"));
    levels->addWidget(rowLevels);

    DaleDFB::comaGetComponent("SoundComponent", &_soundComponent);

    _music = new Music(ILIXI_DATADIR"soundmixer/test.wav");
    _music->setRepeat(true);
}

SoundMixer::~SoundMixer()
{
    delete _music;
    if (_soundComponent)
        _soundComponent->Release(_soundComponent);
}

void
SoundMixer::mute()
{
    _volSlider->setValue(0);
}

void
SoundMixer::playTestSound()
{
    if (_music->status() == Music::Playing)
        _music->stop();
    else
        _music->play();
}

void
SoundMixer::changeVolume(int volume)
{
    if (_soundComponent)
    {
        void *ptr;
        DaleDFB::comaGetLocal(sizeof(int), &ptr);
        int* vol = (int*) ptr;
        *vol = volume;
        DaleDFB::comaCallComponent(_soundComponent, 0, (void*) vol);
    }

    SoundDFB::setMasterVolume(_volSlider->value() / 100);
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    ilixi::SoundMixer app(argc, argv);
    app.exec();
    return 0;
}
