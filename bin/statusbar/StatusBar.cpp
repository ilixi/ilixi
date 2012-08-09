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

    if (vol < 30)
        bar->_sound->setState(0);
    else if (vol < 60)
        bar->_sound->setState(2);
    else
        bar->_sound->setState(4);

    ILOG_DEBUG(ILX_STATUSBAR, "Volume %d\n", *((int*) arg));
}

void
homeShowing(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    bar->_home->setState(1);
}

void
homeHidden(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    bar->_home->setState(0);
}

void
switcherShowing(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    bar->_switch->setState(1);
}

void
switcherHidden(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    bar->_switch->setState(0);
}

StatusBar::StatusBar(int argc, char* argv[])
        : Application(&argc, &argv, (AppOptions) (OptStatusBar | OptDale))
{
    setTitle("StatusBar");
    setBackgroundFilled(true);
    setBackgroundImage(ILIXI_DATADIR"statusbar/statusbar_bg.png");
    setMargins(0, 0, 0, 0);
    HBoxLayout* mainLayout = new HBoxLayout();
    mainLayout->setVerticalAlignment(Alignment::Middle);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);

    _home = new StatusbarButton();
    _home->addImage(new Image(ILIXI_DATADIR"statusbar/home.png", Size(48, 48)));
    _home->addImage(
            new Image(ILIXI_DATADIR"statusbar/homeG.png", Size(48, 48)));
    _home->setState(1);
    _home->sigClicked.connect(sigc::mem_fun(this, &StatusBar::clickedHome));
    addWidget(_home);

    _switch = new StatusbarButton();
    _switch->addImage(
            new Image(ILIXI_DATADIR"statusbar/switch.png", Size(48, 48)));
    _switch->addImage(
            new Image(ILIXI_DATADIR"statusbar/switchG.png", Size(48, 48)));
    _switch->sigClicked.connect(sigc::mem_fun(this, &StatusBar::clickedSwitcher));
    addWidget(_switch);

    _temp = new StatusbarButton();
    _temp->addImage(new Image(ILIXI_DATADIR"statusbar/temp.png", Size(48, 48)));
    _temp->addImage(
            new Image(ILIXI_DATADIR"statusbar/tempG.png", Size(48, 48)));
    addWidget(_temp);

    _sound = new StatusbarButton();
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
    addWidget(_sound);

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
    comaGetComponent("SoundComponent", &_soundComponent);
    _soundComponent->Listen(_soundComponent, 0, volumeListener, this);
    comaGetComponent("CompositorComponent", &_compComponent);
    _compComponent->Listen(_compComponent, 3, homeShowing, this);
    _compComponent->Listen(_compComponent, 4, switcherShowing, this);
    _compComponent->Listen(_compComponent, 5, homeHidden, this);
    _compComponent->Listen(_compComponent, 6, switcherHidden, this);
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
    if (_home->state() % 2 == 0)
        AppBase::comaCallComponent(_compComponent, 3, NULL);
    else
        AppBase::comaCallComponent(_compComponent, 5, NULL);
}

void
StatusBar::clickedSwitcher()
{
    if (_switch->state() % 2 == 0)
        AppBase::comaCallComponent(_compComponent, 4, NULL);
    else
        AppBase::comaCallComponent(_compComponent, 6, NULL);
}

void
StatusBar::clickedTemp()
{

}

void
StatusBar::clickedSound()
{

}

}

int
main(int argc, char* argv[])
{
    ilixi::StatusBar app(argc, argv);
    app.exec();
    return 0;
}
