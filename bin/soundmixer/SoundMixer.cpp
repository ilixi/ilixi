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
#include "VolumeMeter.h"
#include "BandSlider.h"

#include <ui/TabPanel.h>
#include <ui/GroupBox.h>
#include <ui/VBoxLayout.h>
#include <ui/HBoxLayout.h>
#include <ui/GridLayout.h>
#include <ui/PushButton.h>
#include <ui/CheckBox.h>
#include <ui/Spacer.h>

#include <core/ComponentData.h>
#include <types/Music.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_APPMIXER, "ilixi/Mixer", "SoundMixer");

void
volumeListener(void* ctx, void* arg)
{
    ILXSoundMixer* bar = (ILXSoundMixer*) ctx;
    float vol = *((float*) arg);
    bar->_volSlider->setValue(vol);
}

ILXSoundMixer::ILXSoundMixer(int argc, char* argv[])
        : Application(&argc, &argv, AppOptions(OptDale | OptSound)),
          _soundComponent(NULL),
          _music(NULL),
          _bandSliderFont(NULL)
{
    setMargin(20);
    setLayout(new VBoxLayout());

    _bandSliderFont = new Font("Sans", 8);

    HBoxLayout* volStuff = new HBoxLayout();
    addWidget(volStuff);
    TabPanel* panel = new TabPanel();
    volStuff->addWidget(panel);

    //**********
    HBoxLayout* volHBox = new HBoxLayout();
    panel->addPage(volHBox, "Volume Control");

    GridLayout* volGrid = new GridLayout(6, 3);
    volHBox->addWidget(volGrid);
    volGrid->addWidget(new Label("Output:"), 0, 0, 0, 3);

    _volSlider = new Slider();
    _volSlider->setRange(0, 1);
    _volSlider->setValue(1);
    _volSlider->setStep(0.1);
    _volSlider->sigValueChanged.connect(sigc::mem_fun(this, &ILXSoundMixer::changeVolume));
    volGrid->addWidget(_volSlider, 1, 0, 0, 2);

    _mute = new PushButton("Mute");
    _mute->setXConstraint(FixedConstraint);
    volGrid->addWidget(_mute, 1, 2);
    _mute->sigClicked.connect(sigc::mem_fun(this, &ILXSoundMixer::mute));

    volGrid->addWidget(new Label("Balance:"), 2, 0, 0, 3);
    volGrid->addWidget(new Label("Front"), 3, 0);
    Slider* frSlider = new Slider();
    frSlider->setValue(50);
    volGrid->addWidget(frSlider, 3, 1);
    volGrid->addWidget(new Label("Rear"), 3, 2);
    volGrid->addWidget(new Label("Left"), 4, 0);
    Slider* lrSlider = new Slider();
    lrSlider->setValue(50);
    volGrid->addWidget(lrSlider, 4, 1);
    volGrid->addWidget(new Label("Right"), 4, 2);
    volGrid->addWidget(new Spacer(Vertical), 5, 0);

    GroupBox* volMeter = new GroupBox("Meter");
    VBoxLayout* volMeterLayout = new VBoxLayout();
    volMeterLayout->setHorizontalAlignment(Alignment::Center);
    volMeter->setLayout(volMeterLayout);
    volMeter->setXConstraint(FixedConstraint);
    volStuff->addWidget(volMeter);
    VolumeMeter* meter = new VolumeMeter();
    volMeter->addWidget(meter);
    volHBox->addWidget(volMeter);

    //*********
    VBoxLayout* eqBox = new VBoxLayout();
    panel->addPage(eqBox, "Equalizer");

    HBoxLayout* buttons = new HBoxLayout();
    buttons->addWidget(new CheckBox("EQ Enabled"));
    buttons->addWidget(new Spacer(Horizontal));
    buttons->addWidget(new PushButton("Load Preset"));
    buttons->addWidget(new PushButton("Save Preset"));
    PushButton* testSound = new PushButton("Test");
    testSound->sigClicked.connect(sigc::mem_fun(this, &ILXSoundMixer::playTestSound));
    buttons->addWidget(testSound);
    eqBox->addWidget(buttons);

    HBoxLayout* rowLevels = new HBoxLayout();
    rowLevels->setYConstraint(ExpandingConstraint);
    rowLevels->addWidget(new BandSlider("50Hz", _bandSliderFont));
    rowLevels->addWidget(new BandSlider("100Hz", _bandSliderFont));
    rowLevels->addWidget(new BandSlider("156Hz", _bandSliderFont));
    rowLevels->addWidget(new BandSlider("220Hz", _bandSliderFont));
    rowLevels->addWidget(new BandSlider("311Hz", _bandSliderFont));
    rowLevels->addWidget(new BandSlider("440Hz", _bandSliderFont));
    rowLevels->addWidget(new BandSlider("622Hz", _bandSliderFont));
    rowLevels->addWidget(new BandSlider("880Hz", _bandSliderFont));
    rowLevels->addWidget(new BandSlider("1.25KHz", _bandSliderFont));
    rowLevels->addWidget(new BandSlider("1.75KHz", _bandSliderFont));
    rowLevels->addWidget(new BandSlider("2.5KHz", _bandSliderFont));
    rowLevels->addWidget(new BandSlider("3.5KHz", _bandSliderFont));
    rowLevels->addWidget(new BandSlider("5KHz", _bandSliderFont));
    rowLevels->addWidget(new BandSlider("10KHz", _bandSliderFont));
    rowLevels->addWidget(new BandSlider("20KHz", _bandSliderFont));
    eqBox->addWidget(rowLevels);

    DaleDFB::comaGetComponent("SoundMixer", &_soundComponent);
    if (_soundComponent)
        _soundComponent->Listen(_soundComponent, SoundMixer::VolumeChanged, volumeListener, this);

    _music = new Music(ILIXI_DATADIR"soundmixer/test.wav");
    _music->setRepeat(true);

}

ILXSoundMixer::~ILXSoundMixer()
{
    delete _bandSliderFont;
    delete _music;
    if (_soundComponent)
        _soundComponent->Release(_soundComponent);
}

void
ILXSoundMixer::mute()
{
    _volSlider->setValue(0);
    if (_soundComponent)
        DaleDFB::comaCallComponent(_soundComponent, SoundMixer::ToggleMute, NULL);
}

void
ILXSoundMixer::playTestSound()
{
    if (_music->status() == Music::Playing)
        _music->stop();
    else
        _music->play();
}

void
ILXSoundMixer::changeVolume(float volume)
{
    if (_soundComponent && _volSlider->hasFocus())
    {
        void *ptr;
        DaleDFB::comaGetLocal(sizeof(float), &ptr);
        float* vol = (float*) ptr;
        *vol = volume;
        DaleDFB::comaCallComponent(_soundComponent, SoundMixer::SetVolume, (void*) vol);
    }
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    ilixi::ILXSoundMixer app(argc, argv);
    app.exec();
    return 0;
}
