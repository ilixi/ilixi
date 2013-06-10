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

#include "Home.h"
#include "AppButton.h"
#include <ui/HBoxLayout.h>
#include <core/Logger.h>
#include <sigc++/bind.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_HOMEAPP, "ilixi/phone/PHome", "PHome");

void
appStatusChanged(void* ctx, void* arg)
{
    ILOG_TRACE_F(ILX_HOMEAPP);
    PHome* home = (PHome*) ctx;
    Compositor::VisibilityData notification = *((Compositor::VisibilityData*) arg);

    PAppButton* button = NULL;
    for (PHome::AppButtonVector::iterator it = home->_buttons.begin(); it != home->_buttons.end(); ++it)
    {
        if (((PAppButton*) *it)->text() == notification.name)
            button = ((PAppButton*) *it);
    }

    if (button)
    {
        if (notification.status & Compositor::AppVisible)
            button->setAppVisible(true);
        else if (notification.status & Compositor::AppHidden)
            button->setAppVisible(false);
        else if (notification.status & Compositor::AppStarting)
            button->appStarting();
        else if (notification.status & Compositor::AppQuit)
            button->appQuit();
    }
}

void
receiveAppList(void* ctx, void* arg)
{
    ILOG_TRACE_F(ILX_HOMEAPP);
    PHome* home = (PHome*) ctx;

    int* size = ((int*) arg);
    ILOG_DEBUG(ILX_HOMEAPP, " -> Size: %d\n", *size);
    Compositor::AppData *srcdata = (Compositor::AppData*) (size + 1);
    PHome::AppDataVector vec;

    for (int i = 0; i < *size; ++i)
    {
        ILOG_DEBUG(ILX_HOMEAPP, "%s - %s\n", srcdata[i].name, srcdata[i].icon);

        Compositor::AppData data;
        snprintf(data.name, 64, "%s", srcdata[i].name);
        snprintf(data.icon, 128, "%s", srcdata[i].icon);
        vec.push_back(data);
    }
    home->initButtons(vec);
}

PHome::PHome(int argc, char* argv[])
        : Application(&argc, &argv, OptDaleAuto),
          _compositor(NULL)
{
    ILOG_TRACE_F(ILX_HOMEAPP);

    setMargin(10);
    setLayout(new HBoxLayout());

    _view = new GridView();
    _view->setGridSize(10, 4);
    _view->setUseThumbs(true);
    addWidget(_view);

    DaleDFB::comaGetComponent("Compositor", &_compositor);

//    sigGeometryUpdated.connect(sigc::mem_fun(this, &PHome::updateHomeGeometry));
    sigVisible.connect(sigc::mem_fun(this, &PHome::requestAppList));
}

PHome::~PHome()
{
    ILOG_TRACE_F(ILX_HOMEAPP);
    if (_compositor)
        _compositor->Release(_compositor);
}

void
PHome::runApp(const std::string& name)
{
    ILOG_TRACE_F(ILX_HOMEAPP);
    if (_compositor)
    {
        void *ptr;
        DaleDFB::comaGetLocal(128, &ptr);
        char* n = (char*) ptr;
        snprintf(n, 128, "%s", name.c_str());
        DaleDFB::comaCallComponent(_compositor, Compositor::StartApp, (void*) n);
    }
}

void
PHome::initButtons(const PHome::AppDataVector& dataVector)
{
    ILOG_TRACE_F(ILX_HOMEAPP);
    for (PHome::AppDataVector::const_iterator it = dataVector.begin(); it != dataVector.end(); ++it)
    {
        PAppButton* button = new PAppButton(((Compositor::AppData) *it).name);
        button->setToolButtonStyle(ToolButton::IconAboveText);
        button->setIcon(((Compositor::AppData) *it).icon, Size(96, 96));
        button->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(this, &PHome::runApp), button->text()));
//        button->sigClicked.connect(sigc::mem_fun(button, &PAppButton::appStarting));

        _view->addItem(button);
        _buttons.push_back(button);
    }
    appWindow()->eventManager()->selectNext();
}

void
PHome::requestAppList()
{
    ILOG_TRACE_F(ILX_HOMEAPP);
    if (_compositor)
    {
        _compositor->Listen(_compositor, Compositor::AppStatus, appStatusChanged, this);
        _compositor->Listen(_compositor, Compositor::SendingAppList, receiveAppList, this);
        DaleDFB::comaCallComponent(_compositor, Compositor::GetAppList, NULL);
    }
}

void
PHome::updateHomeGeometry()
{
    ILOG_TRACE_F(ILX_HOMEAPP);
    _view->setGeometry(10, 10, width() - 20, height() - 20);
}

}

int
main(int argc, char* argv[])
{
    ilixi::PHome app(argc, argv);
    app.exec();
    return 0;
}
