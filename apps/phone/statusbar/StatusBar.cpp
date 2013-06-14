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
#include <ui/VBoxLayout.h>
#include <ui/HBoxLayout.h>
#include <graphics/Painter.h>
#include <core/Logger.h>
#include <core/ComponentData.h>
#include "Clock.h"
#include "ListItem.h"

D_DEBUG_DOMAIN( ILX_STATUSBAR, "ilixi/StatusBar", "StatusBar");

namespace ilixi
{

void
volumeListener(void* ctx, void* arg)
{
    PStatusBar* bar = (PStatusBar*) ctx;
    float vol = *((float*) arg);

    if (vol == 0)
        bar->_vol->setState(0);
    else if (vol < .30)
        bar->_vol->setState(1);
    else if (vol < .60)
        bar->_vol->setState(2);
}

void
appStatusChanged(void* ctx, void* arg)
{
    PStatusBar* home = (PStatusBar*) ctx;
    Compositor::VisibilityData notification = *((Compositor::VisibilityData*) arg);
    home->appWindow()->eventManager()->setFocusedWidget(NULL);
}

//*****************************************************************

PStatusBar::PStatusBar(int argc, char* argv[])
        : Application(&argc, &argv, (AppOptions) (OptDaleAuto)),
          _bg(NULL),
          _listFont(NULL),
          _soundComponent(NULL),
          _compComponent(NULL)
{
    _bg = new Image(ILIXI_DATADIR"phone/statusbar/bg.png");
    appWindow()->setCustomCompose(true);

    setMargins(15, 45, 15, 15);
    VBoxLayout* mainLayout = new VBoxLayout();
    mainLayout->setHorizontalAlignment(Alignment::Center);
    mainLayout->setSpacing(5);
    setLayout(mainLayout);

    Clock* clock = new Clock();
    addWidget(clock);

    HBoxLayout* iconLayout = new HBoxLayout();
    iconLayout->setYConstraint(FixedConstraint);
    addWidget(iconLayout);

    _headset = new NotificationIcon();
    _headset->addState(ILIXI_DATADIR"phone/statusbar/headset.png");
    iconLayout->addWidget(_headset);

    _mic = new NotificationIcon();
    _mic->addState(ILIXI_DATADIR"phone/statusbar/mic.png");
    iconLayout->addWidget(_mic);

    _vol = new NotificationIcon();
    _vol->addState(ILIXI_DATADIR"phone/statusbar/vol0.png");
    _vol->addState(ILIXI_DATADIR"phone/statusbar/vol1.png");
    _vol->addState(ILIXI_DATADIR"phone/statusbar/vol2.png");
    iconLayout->addWidget(_vol);

    _list = new ListBox();
    _list->setUseThumbs(true);
    _list->setYConstraint(ExpandingConstraint);
    addWidget(_list);

    _fpsLabel = new Label("");
    _fpsLabel->setLayoutAlignment(TextLayout::Center);
    addWidget(_fpsLabel);

    _cpuMon = new CPUMonitor();

    _cpuChart = new BarChart(10);
    _cpuChart->addBar("CPU Total", Color(28, 127, 192, 100));
    _cpuChart->setMaximumSize(150, 50);
    _cpuChart->setDrawBG(false);
    addWidget(_cpuChart);

    sigVisible.connect(sigc::mem_fun(this, &PStatusBar::onShow));
}

PStatusBar::~PStatusBar()
{
    delete _timer;
    delete _bg;
    delete _listFont;
    delete _listBG;
    _compComponent->Release(_compComponent);
    _soundComponent->Release(_soundComponent);
}

void
PStatusBar::onShow()
{
    _listFont = new Font("Sans", 12);
    _listBG = new Image(ILIXI_DATADIR"phone/statusbar/item-box.png");

    ListItem* item = new ListItem("Home");
    item->setFont(_listFont);
    item->setBg(_listBG);
    item->setIcon(ILIXI_DATADIR"phone/statusbar/home.png", Size(32, 32));
    item->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(this, &PStatusBar::showApp), "Home"));
    _list->addItem(item);

    item = new ListItem("Switcher");
    item->setFont(_listFont);
    item->setBg(_listBG);
    item->setIcon(ILIXI_DATADIR"phone/statusbar/switch.png", Size(32, 32));
    item->sigClicked.connect(sigc::mem_fun(this, &PStatusBar::showSwitcher));
    _list->addItem(item);

    item = new ListItem("Dialer");
    item->setFont(_listFont);
    item->setBg(_listBG);
    item->setIcon(ILIXI_DATADIR"phone/statusbar/dialer.png", Size(32, 32));
    item->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(this, &PStatusBar::showApp), "Dialer"));
    _list->addItem(item);

    item = new ListItem("SoundMixer");
    item->setFont(_listFont);
    item->setBg(_listBG);
    item->setIcon(ILIXI_DATADIR"phone/statusbar/soundmixer.png", Size(32, 32));
    item->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(this, &PStatusBar::showApp), "SoundMixer"));
    _list->addItem(item);

    item = new ListItem("Settings");
    item->setFont(_listFont);
    item->setBg(_listBG);
    item->setIcon(ILIXI_DATADIR"phone/statusbar/settings.png", Size(32, 32));
    item->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(this, &PStatusBar::showApp), "Settings"));
    _list->addItem(item);

    _timer = new Timer();
    _timer->sigExec.connect(sigc::mem_fun(this, &PStatusBar::timerSlot));
    _timer->start(1000);

    DaleDFB::comaGetComponent("SoundMixer", &_soundComponent);
    DaleDFB::comaGetComponent("Compositor", &_compComponent);
    if (_compComponent)
        _compComponent->Listen(_compComponent, Compositor::AppStatus, appStatusChanged, this);

    if (_soundComponent)
        _soundComponent->Listen(_soundComponent, SoundMixer::VolumeChanged, volumeListener, this);
}

void
PStatusBar::showApp(const std::string& title)
{
    if (_compComponent)
    {
        void *ptr;
        DaleDFB::comaGetLocal(128, &ptr);
        char* n = (char*) ptr;
        snprintf(n, 128, "%s", title.c_str());
        DaleDFB::comaCallComponent(_compComponent, Compositor::StartApp, (void*) n);
    }
}

void
PStatusBar::compose(const PaintEvent& event)
{
    Painter p(appWindow());
    p.begin(event);
    p.blitImage(_bg, Rectangle(0, 0, 150, 20), 0, 0);
    p.stretchImage(_bg, Rectangle(0, 20, 150, height() - 80), Rectangle(0, 20, 150, 380));
    p.blitImage(_bg, Rectangle(0, 400, 150, 80), 0, height() - 80);
}

void
PStatusBar::timerSlot()
{
    float fps = 0;
    if (_compComponent)
    {
        void *ptr;
        DaleDFB::comaGetLocal(sizeof(float), &ptr);
        DaleDFB::comaCallComponent(_compComponent, Compositor::GetFPS, (void*) ptr);
        fps = *((float*) ptr);
    }

    _fpsLabel->setText(PrintF("FPS: %.1f", fps));
    _cpuMon->refresh();
    _cpuChart->addValue(0, _cpuMon->getCpu(0).getUsage());
}

void
PStatusBar::showSwitcher()
{
    if (_compComponent)
        DaleDFB::comaCallComponent(_compComponent, Compositor::ShowSwitcher, NULL);
}

}

int
main(int argc, char* argv[])
{
    ilixi::PStatusBar app(argc, argv);
    app.exec();
    return 0;
}
