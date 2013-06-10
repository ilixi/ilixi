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

#include "StatusBar.h"
#include <ui/HBoxLayout.h>
#include <graphics/Painter.h>
#include <core/Logger.h>
#include <sigc++/sigc++.h>
#include <string.h>
#include "Clock.h"
#include "StatusbarComponent.h"

D_DEBUG_DOMAIN( ILX_STATUSBAR, "ilixi/car/status", "StatusBar");

namespace ilixi
{

void
volumeListener(void* ctx, void* arg)
{
    ILXStatusBar* bar = (ILXStatusBar*) ctx;
    float vol = *((float*) arg);

    if (vol == 0)
        bar->_sound->setButtonState(0);
    else if (vol < .30)
        bar->_sound->setButtonState(2);
    else if (vol < .60)
        bar->_sound->setButtonState(4);
    else
        bar->_sound->setButtonState(6);
}

void
appVisibilty(void* ctx, void* arg)
{
    ILXStatusBar* bar = (ILXStatusBar*) ctx;
    Compositor::VisibilityData data = *((Compositor::VisibilityData*) arg);

    if (strcmp(data.name, "Home") == 0)
    {
        if (data.status & Compositor::AppVisible)
            bar->_home->setActive(1);
        else
            bar->_home->setActive(0);
    } else if (strcmp(data.name, "SoundMixer") == 0)
    {
        if (data.status & Compositor::AppVisible)
            bar->_sound->setActive(1);
        else
            bar->_sound->setActive(0);

    } else if (strcmp(data.name, "Dashboard") == 0)
    {
        if (data.status & Compositor::AppVisible)
            bar->_dash->setActive(1);
        else
            bar->_dash->setActive(0);
    }
}

void
switcherShowing(void* ctx, void* arg)
{
    ILXStatusBar* bar = (ILXStatusBar*) ctx;
    bar->_switch->setActive(1);
}

void
switcherHidden(void* ctx, void* arg)
{
    ILXStatusBar* bar = (ILXStatusBar*) ctx;
    bar->_switch->setActive(0);
}

void
backVisible(void* ctx, void* arg)
{
    ILXStatusBar* bar = (ILXStatusBar*) ctx;
    bar->_back->setVisible(true);
    bar->update();
}

void
backHidden(void* ctx, void* arg)
{
    ILXStatusBar* bar = (ILXStatusBar*) ctx;
    bar->_back->setVisible(false);
    bar->update();
}

//*****************************************************************

ILXStatusBar::ILXStatusBar(int argc, char* argv[])
        : Application(&argc, &argv, (AppOptions) (OptDaleAuto))
{
    setBackgroundImage(ILIXI_DATADIR"car/statusbar/statusbar_bg.png");

    setMargins(0, 0, 0, 0);
    HBoxLayout* mainLayout = new HBoxLayout();
    mainLayout->setVerticalAlignment(Alignment::Middle);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);

    _home = new StatusbarButton();
    _home->addImage(new Image(ILIXI_DATADIR"car/statusbar/home.png", Size(48, 48)));
    _home->addImage(new Image(ILIXI_DATADIR"car/statusbar/homeG.png", Size(48, 48)));
    _home->setActive(1);
    _home->sigClicked.connect(sigc::mem_fun(this, &ILXStatusBar::clickedHome));
    addWidget(_home);

    _switch = new StatusbarButton();
    _switch->addImage(new Image(ILIXI_DATADIR"car/statusbar/switch.png", Size(48, 48)));
    _switch->addImage(new Image(ILIXI_DATADIR"car/statusbar/switchG.png", Size(48, 48)));
    _switch->sigClicked.connect(sigc::mem_fun(this, &ILXStatusBar::clickedSwitcher));
    addWidget(_switch);

    _dash = new StatusbarButton();
    _dash->addImage(new Image(ILIXI_DATADIR"car/statusbar/dash.png", Size(48, 48)));
    _dash->addImage(new Image(ILIXI_DATADIR"car/statusbar/dashG.png", Size(48, 48)));
    _dash->sigClicked.connect(sigc::mem_fun(this, &ILXStatusBar::clickedDash));
    addWidget(_dash);

    _sound = new StatusbarButton();
    _sound->addImage(new Image(ILIXI_DATADIR"car/statusbar/vol0.png", Size(48, 48)));
    _sound->addImage(new Image(ILIXI_DATADIR"car/statusbar/vol0G.png", Size(48, 48)));
    _sound->addImage(new Image(ILIXI_DATADIR"car/statusbar/vol1.png", Size(48, 48)));
    _sound->addImage(new Image(ILIXI_DATADIR"car/statusbar/vol1G.png", Size(48, 48)));
    _sound->addImage(new Image(ILIXI_DATADIR"car/statusbar/vol2.png", Size(48, 48)));
    _sound->addImage(new Image(ILIXI_DATADIR"car/statusbar/vol2G.png", Size(48, 48)));
    _sound->addImage(new Image(ILIXI_DATADIR"car/statusbar/vol3.png", Size(48, 48)));
    _sound->addImage(new Image(ILIXI_DATADIR"car/statusbar/vol3G.png", Size(48, 48)));
    _sound->sigClicked.connect(sigc::mem_fun(this, &ILXStatusBar::clickedSound));
    _sound->setButtonState(6);
    addWidget(_sound);

    _back = new StatusbarButton();
    _back->setVisible(false);
    _back->addImage(new Image(ILIXI_DATADIR"car/statusbar/back0.png", Size(48, 48)));
    _back->sigClicked.connect(sigc::mem_fun(this, &ILXStatusBar::clickedBack));
    addWidget(_back);

    addWidget(new Clock());

    _rca = new RemoteContentArea();
    addWidget(_rca);

    sigVisible.connect(sigc::mem_fun(this, &ILXStatusBar::onShow));
    sigHidden.connect(sigc::mem_fun(this, &ILXStatusBar::onHide));

    _statComp = new StatusbarComponent(this);

}

ILXStatusBar::~ILXStatusBar()
{
    delete _bg;
}

void
ILXStatusBar::onHide()
{
    _soundComponent->Release(_soundComponent);
}

void
ILXStatusBar::onShow()
{
    DaleDFB::comaGetComponent("SoundMixer", &_soundComponent);
    DaleDFB::comaGetComponent("Compositor", &_compComponent);

    _soundComponent->Listen(_soundComponent, SoundMixer::VolumeChanged, volumeListener, this);

    _compComponent->Listen(_compComponent, Compositor::AppStatus, appVisibilty, this);

    _compComponent->Listen(_compComponent, Compositor::SwitcherVisible, switcherShowing, this);
    _compComponent->Listen(_compComponent, Compositor::SwitcherHidden, switcherHidden, this);

    _compComponent->Listen(_compComponent, Compositor::BackKeyVisible, backVisible, this);
    _compComponent->Listen(_compComponent, Compositor::BackKeyHidden, backHidden, this);
}

bool
ILXStatusBar::addRemoteContent(DFBSurfaceID id)
{
    return _rca->addRemoteContent(id);
}

bool
ILXStatusBar::removeRemoteContent(DFBSurfaceID id)
{
    return _rca->removeRemoteContent(id);
}

void
ILXStatusBar::clickedHome()
{
    if (_home->active())
        DaleDFB::comaCallComponent(_compComponent, Compositor::HideHome, NULL);
    else
        DaleDFB::comaCallComponent(_compComponent, Compositor::ShowHome, NULL);
}

void
ILXStatusBar::clickedSwitcher()
{
    if (_switch->active())
        DaleDFB::comaCallComponent(_compComponent, Compositor::HideSwitcher, NULL);
    else
        DaleDFB::comaCallComponent(_compComponent, Compositor::ShowSwitcher, NULL);
}

void
ILXStatusBar::clickedDash()
{
    if (_dash->active())
        DaleDFB::comaCallComponent(_compComponent, Compositor::HideHome, NULL);
    else
    {
        void *ptr;
        DaleDFB::comaGetLocal(128, &ptr);
        char* n = (char*) ptr;
        snprintf(n, 128, "Dashboard");
        DaleDFB::comaCallComponent(_compComponent, Compositor::StartApp, (void*) n);
    }
}

void
ILXStatusBar::clickedSound()
{
    if (_sound->active())
        DaleDFB::comaCallComponent(_compComponent, Compositor::HideHome, NULL);
    else
    {
        void *ptr;
        DaleDFB::comaGetLocal(128, &ptr);
        char* n = (char*) ptr;
        snprintf(n, 128, "SoundMixer");
        DaleDFB::comaCallComponent(_compComponent, Compositor::StartApp, (void*) n);
    }
}

void
ILXStatusBar::clickedBack()
{
    DaleDFB::comaCallComponent(_compComponent, Compositor::SendBackKey, NULL);
}

}

int
main(int argc, char* argv[])
{
    ilixi::ILXStatusBar app(argc, argv);
    app.exec();
    return 0;
}
