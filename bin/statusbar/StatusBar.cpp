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

D_DEBUG_DOMAIN( ILX_STATUSBAR, "ilixi/StatusBar", "StatusBar");

namespace ilixi
{

void
volumeListener(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    int vol = *((int*) arg);

    if (vol == 0)
        bar->_sound->setButtonState(0);
    else if (vol < 30)
        bar->_sound->setButtonState(2);
    else if (vol < 60)
        bar->_sound->setButtonState(4);
    else
        bar->_sound->setButtonState(6);

    ILOG_DEBUG(ILX_STATUSBAR, "Volume %d\n", *((int*) arg));
}

void
homeShowing(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    bar->_home->setActive(1);
    bar->_sound->setActive(0);
    bar->_dash->setActive(0);
}

void
homeHidden(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    bar->_home->setActive(0);
}

void
switcherShowing(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    bar->_switch->setActive(1);
}

void
switcherHidden(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    bar->_switch->setActive(0);
}

void
soundVisible(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    bar->_sound->setActive(1);
}

void
soundHidden(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    bar->_sound->setActive(0);
}

void
dashVisible(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    bar->_dash->setActive(1);
}

void
dashHidden(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    bar->_dash->setActive(0);
}

void
backVisible(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    bar->_back->setVisible(true);
    bar->update();
}

void
backHidden(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    bar->_back->setVisible(false);
    bar->update();
}

//*****************************************************************

StatusBar::StatusBar(int argc, char* argv[])
        : Application(&argc, &argv, (AppOptions) (OptDale))
{
    setTitle("StatusBar");
    setBackgroundFilled(true);
    setBackgroundImage(ILIXI_DATADIR"statusbar/statusbar_bg.png");
    setPaletteFromFile(ILIXI_DATADIR"statusbar/def_palette.xml");

    setMargins(0, 0, 0, 0);
    HBoxLayout* mainLayout = new HBoxLayout();
    mainLayout->setVerticalAlignment(Alignment::Middle);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);

    _home = new StatusbarButton();
    _home->addImage(new Image(ILIXI_DATADIR"statusbar/home.png", Size(48, 48)));
    _home->addImage(
            new Image(ILIXI_DATADIR"statusbar/homeG.png", Size(48, 48)));
    _home->setActive(1);
    _home->sigClicked.connect(sigc::mem_fun(this, &StatusBar::clickedHome));
    addWidget(_home);

    _switch = new StatusbarButton();
    _switch->addImage(
            new Image(ILIXI_DATADIR"statusbar/switch.png", Size(48, 48)));
    _switch->addImage(
            new Image(ILIXI_DATADIR"statusbar/switchG.png", Size(48, 48)));
    _switch->sigClicked.connect(
            sigc::mem_fun(this, &StatusBar::clickedSwitcher));
    addWidget(_switch);

    _dash = new StatusbarButton();
    _dash->addImage(new Image(ILIXI_DATADIR"statusbar/dash.png", Size(48, 48)));
    _dash->addImage(
            new Image(ILIXI_DATADIR"statusbar/dashG.png", Size(48, 48)));
    _dash->sigClicked.connect(sigc::mem_fun(this, &StatusBar::clickedDash));
    addWidget(_dash);

    _sound = new StatusbarButton();
    _sound->addImage(
            new Image(ILIXI_DATADIR"statusbar/vol0.png", Size(48, 48)));
    _sound->addImage(
            new Image(ILIXI_DATADIR"statusbar/vol0G.png", Size(48, 48)));
    _sound->addImage(
            new Image(ILIXI_DATADIR"statusbar/vol1.png", Size(48, 48)));
    _sound->addImage(
            new Image(ILIXI_DATADIR"statusbar/vol1G.png", Size(48, 48)));
    _sound->addImage(
            new Image(ILIXI_DATADIR"statusbar/vol2.png", Size(48, 48)));
    _sound->addImage(
            new Image(ILIXI_DATADIR"statusbar/vol2G.png", Size(48, 48)));
    _sound->addImage(
            new Image(ILIXI_DATADIR"statusbar/vol3.png", Size(48, 48)));
    _sound->addImage(
            new Image(ILIXI_DATADIR"statusbar/vol3G.png", Size(48, 48)));
    _sound->sigClicked.connect(sigc::mem_fun(this, &StatusBar::clickedSound));
    _sound->setButtonState(6);
    addWidget(_sound);

    _back = new StatusbarButton();
    _back->setVisible(false);
    _back->addImage(
            new Image(ILIXI_DATADIR"statusbar/back0.png", Size(48, 48)));
    _back->sigClicked.connect(sigc::mem_fun(this, &StatusBar::clickedBack));
    addWidget(_back);

    addWidget(new Clock());

    _rca = new RemoteContentArea();
    addWidget(_rca);

    sigVisible.connect(sigc::mem_fun(this, &StatusBar::onShow));
    sigHidden.connect(sigc::mem_fun(this, &StatusBar::onHide));

    _statComp = new StatusbarComponent(this);

}

StatusBar::~StatusBar()
{
    delete _bg;
}

void
StatusBar::onHide()
{
    _soundComponent->Release(_soundComponent);
}

void
StatusBar::onShow()
{
    DaleDFB::comaGetComponent("SoundComponent", &_soundComponent);
    _soundComponent->Listen(_soundComponent, 0, volumeListener, this);
    DaleDFB::comaGetComponent("CompositorComponent", &_compComponent);
    _compComponent->Listen(_compComponent, 3, homeShowing, this);
    _compComponent->Listen(_compComponent, 4, switcherShowing, this);
    _compComponent->Listen(_compComponent, 5, homeHidden, this);
    _compComponent->Listen(_compComponent, 6, switcherHidden, this);

    _compComponent->Listen(_compComponent, 8, soundVisible, this);
    _compComponent->Listen(_compComponent, 9, soundHidden, this);
    _compComponent->Listen(_compComponent, 10, dashVisible, this);
    _compComponent->Listen(_compComponent, 11, dashHidden, this);

    _compComponent->Listen(_compComponent, 12, backVisible, this);
    _compComponent->Listen(_compComponent, 13, backHidden, this);

}

bool
StatusBar::addRemoteContent(DFBSurfaceID id)
{
    return _rca->addRemoteContent(id);
}

bool
StatusBar::removeRemoteContent(DFBSurfaceID id)
{
    return _rca->removeRemoteContent(id);
}

void
StatusBar::clickedHome()
{
    if (_home->active())
        DaleDFB::comaCallComponent(_compComponent, 5, NULL);
    else
        DaleDFB::comaCallComponent(_compComponent, 3, NULL);
}

void
StatusBar::clickedSwitcher()
{
    if (_switch->active())
        DaleDFB::comaCallComponent(_compComponent, 6, NULL);
    else
        DaleDFB::comaCallComponent(_compComponent, 4, NULL);
}

void
StatusBar::clickedDash()
{
    if (_dash->active())
        DaleDFB::comaCallComponent(_compComponent, 12, NULL);
    else
        DaleDFB::comaCallComponent(_compComponent, 11, NULL);
}

void
StatusBar::clickedSound()
{
    if (_sound->active())
        DaleDFB::comaCallComponent(_compComponent, 10, NULL);
    else
        DaleDFB::comaCallComponent(_compComponent, 9, NULL);
}

void
StatusBar::clickedBack()
{
    DaleDFB::comaCallComponent(_compComponent, 13, NULL);
}

}

int
main(int argc, char* argv[])
{
    ilixi::StatusBar app(argc, argv);
    app.exec();
    return 0;
}
